
#include "UserType.h"
#include "FunctionAttributes.h"
#include "HeaderGlobals.h"
#include "Identifier.h"
#include "Interface.h"
#include "FunctionImpl.h"
#include "FunctionDecl.h"
#include "FunctionParams.h"
#include "ConstructorCall.h"
#include "Variable.h"
#include "Visitor.h"
#include "Enum.h"
#include "Load.h"
#include "BackLLVM.h"
#include "Int8.h"
#include "Scalar.h"
#include "Program.h"
#include "PropagateTypes.h"

class ParentScalar: public Scalar {
public:
    ParentScalar(DataType parentDt, location_t loc) : 
        Scalar(Identifier("parent", loc), NULL) {
        Load *load = new Load("_parent", loc);
        load->setDataType(parentDt);
        load->setScope(this);
        setExpr(load);
        dt = parentDt;
        pointer = pm_pointer;
    }
    virtual DataType getDataType() override {
		return dt;
	}
    virtual Type* getLLVMType() override {
        return PointerType::get(global_context, dataAddrSpace);
    }
};

void UserType::setNestedParent() {
    for(auto child : this->children()) {
        if (UserType *ut = dynamic_cast<UserType*>(child)) {
            ut->setParent(this);
        }
    }
}

bool UserType::createDataType() {
    
    if (dt != BuildTypes::undefinedType)
        return true;
    
    // create an undefined type to use in recursive subtypes
    dt = buildTypes->getType(getTypeName(), true);
    std::vector<Type*> elements;

    int idx = 0;
    unsigned startBit = 0;

    // if needed, add typeid as the first field
    UInt8 *typeId = NULL;
    if (implements.size() > 0) {
        typeId = new UInt8(0, getLoc()); // zero is changed below
        Scalar *sc = new Scalar("typeid", typeId);
        sc->setScope(this);
        addChild(sc, true);
        addSymbol(sc);
    }

    for(auto child : this->children()) {
        Variable *v = NULL;

        if (UserType *ut = dynamic_cast<UserType*>(child)) {
            // internal user type:
            // add a var in the internal type to store a reference to his parent
            ParentScalar *s = new ParentScalar(dt, ut->getLoc());
            s->setScope(ut);
            ut->addChild(s);
            ut->addSymbol(s);
            ut->createDataType();

            // create a new var in the parent to store the intf implementation
            ConstructorCall *cc = new ConstructorCall(ut->getTypeName(), ut->getLoc());
            v = new Scalar(ut->getName(), cc);
            v->dt = ut->getDataType();
            cc->setScope(v);
            v->setScope(this);
            addChild(v);
            
            // replace the v->getName() symbol with its var and
            symbols[v->getName()] = v;

            // add the nested type to the program root
            program->addSymbol(ut->getTypeName(), ut);

        } else {
            v = dynamic_cast<Variable*>(child);
        }

        if (v && !v->isConstExpr()) {
            if (PropagateTypes::isUndefined(v, false)) {
                return false;
            } else if (buildTypes->isInterface(v->getDataType())) {
                v->setPointer(pm_pointer);
                v->setPointerToPointer(true);
            } else if (v->getPointerMode() == pm_unknown)
                v->setPointer(pm_nopointer);
            
            Type *llvmType = v->getLLVMType();
            assert(llvmType && "Can not construct a type without its LLVM type.");
            if (!llvmType)
                return false;

            elements.push_back(llvmType);
            v->setGEPIndex(idx++);

            //FIXME: Fix for data alignment on non-packed structure/type
            startBits[v->getName()] = startBit;
            if (llvmType->isPointerTy())
                startBit += buildTypes->getTargetPointerBitWidth();
            else
                startBit += buildTypes->bitWidth(v->getDataType());
        }
    }
    bitWidth = startBit;

    StructType *uttype = StructType::create(global_context, ArrayRef<Type*>(elements), getName());
    dt = buildTypes->addDataType(this, uttype, bitWidth);
    if (parent)
        buildTypes->setInternal(dt, true);

    if (dt == BuildTypes::undefinedType) {
        yyerrorcpp("Type " + getTypeName() + " alread defined.", this);
        yywarncpp(getTypeName() + " was first defined here.", buildTypes->location(dt));
        return false;
    }

    if (typeId)
        typeId->setNumber(dt);

    return true;
}

Value *UserType::generate(FunctionImpl *func, BasicBlock *block, BasicBlock *allocblock) {
    
    if (!createDataType())
        return NULL;
    
    // semantic analysis
    for(auto n : implements) {
        Node *inode = findSymbol(n);
        Interface *intf = dynamic_cast<Interface*>(inode);
        if (!intf) {
            yyerrorcpp("Interface " + n + " not defined.", this);
            return NULL;
        }
        if (!intf->validateImplementation(this))
            return NULL;
    }

    // get vars
    vector<Node*> fields;
    for(auto & [key, stmt] : getSymbols()) {
        if (Variable *c = dynamic_cast<Variable*>(stmt)) {
            if (!c->isConstExpr())
                fields.push_back(c);
        }
    }

    /* set function parameters before generate
     *  A function can call others that are still not generated
     *  and request their type info. So, here we set all needed
     *  info to pre-generate their type.
     */
    for(auto & [key, stmt] : getSymbols()) {
        if (FunctionImpl *f = dynamic_cast<FunctionImpl*>(stmt)) {
            f->setPrefixName(getTypeName());
            f->addThisArgument(dt);
            for(auto &field : fields)
                f->addSymbol(dynamic_cast<NamedNode*>(field));
            f->setExternal(declaration);
        }
    }

    // generate user types and enums
    for(auto* stmt : node_children) {
        if (UserType *ut = dynamic_cast<UserType*>(stmt)) {
            ut->setDeclaration(declaration);
            ut->generate(func, block, allocblock);
        } else if (Enum *en = dynamic_cast<Enum*>(stmt)) {
            en->generate(func, block, allocblock);
        }
    }

    // generate init function/constructor
    FunctionParams *fp = new FunctionParams();
    FunctionBase *finit;
    if (declaration) {
        finit = new FunctionDecl((DataType)tvoid, "init", fp, getLoc());
    } else {
        finit = new FunctionImpl((DataType)tvoid, "init", fp, 
            std::move(fields), getLoc(), getLoc(), true);
    }
    finit->addThisArgument(dt);
    if (parent) {
        finit->addParentArgument(parent->getDataType());
        // nested user types can be inlined in the parent init
        finit->getAttributes()->addAttribute(fa_inline);
    }
    finit->setPrefixName(getTypeName());
    finit->setExternal(declaration);
    finit->setConstructor(true);
    finit->generate(func, block, allocblock);

    // generate functions
    for(auto & [key, stmt] : getSymbols()) {
        if (FunctionImpl *f = dynamic_cast<FunctionImpl*>(stmt)) {
            f->generate(func, block, allocblock);
        }
    }

    addChild(finit);
    addSymbol(finit);
    return NULL;
}

Node* UserType::accept(Visitor& v) {
	return v.visit(*this);
}

unsigned UserType::getFieldStartBit(Node *field) {
    return startBits[field->getName()];
}

const string UserType::getTypeName() const {
    if (parent)
        return parent->getName() + ":" + getName();
    else
        return getName();
}

void UserType::addSymbol(NamedNode *nm) {
    // addSymbol is overrided here because UserType has its
    // own scope and shouldn't call findSymbol recursivelly
    const string& label = nm->getName();
    auto it = symbols.find(label);
    if (it != symbols.end()) {
        yyerrorcpp(string_format("Symbol %s already declared for this type.", label.c_str()), nm);
        yywarncpp("The first declaration is here.", it->second);
    }
    else
        symbols[nm->getName()] = nm;
}
