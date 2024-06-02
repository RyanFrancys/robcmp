#include "FunctionCall.h"
#include "BuildTypes.h"
#include "Cast.h"
#include "FunctionDecl.h"
#include "FunctionImpl.h"
#include "HeaderGlobals.h"
#include "Coercion.h"
#include "Load.h"
#include "Visitor.h"
#include "BackLLVM.h"
#include "UserType.h"
#include "Program.h"
#include "Interface.h"

extern Program *program;

DataType FunctionCall::getDataType() {
    
    if (dt == BuildTypes::undefinedType) {
        // is a cast or constructor?
        dt = buildTypes->getType(ident.getFullName());
        if (parameters->getNumParams() <= 1 && dt != BuildTypes::undefinedType) {
            return dt;
        }

        if (!symbol)
            symbol = ident.getSymbol(getScope());

        if (symbol)
            dt = symbol->getDataType();
    }
    return dt;
}

Value *FunctionCall::generate(FunctionImpl *func, BasicBlock *block, BasicBlock *allocblock) {

    RobDbgInfo.emitLocation(this);
    string name = ident.getFullName();

    /* check if it is a constructor or cast call:
        - cast if the function name is a type, e.g., int8(x), and the call has one argument
        - constructor if the function name is a type and the call has no argument
    */

    DataType adt = buildTypes->getType(name);
    if (adt != BuildTypes::undefinedType) {
        dt = adt;
        unsigned p = parameters->getNumParams();
        // call with only one parameter is a cast
        if (p == 1) {
            Node *param = parameters->getParamElement(0);
            Cast ca(dt, param);
            ca.setScope(this);
            if (!param->getScope())
                param->setScope(&ca);
            return ca.generate(func, block, allocblock);

        } else if (p == 0) { // it's a constructor			
            // alloc
            Builder->SetInsertPoint(block);
            Value *var = leftValue->getLLVMValue(func);
            if (var == NULL) {
                // is a new left var
                Builder->SetInsertPoint(allocblock);
                var = Builder->CreateAlloca(buildTypes->llvmType(dt), dataAddrSpace, 0, 
                    leftValue->getName());
                leftValue->setAlloca(var);
                leftValue->setDataType(dt);
                if (debug_info)
                    RobDbgInfo.declareVar(leftValue, var, allocblock);
            }
            vector<Value*> args;
            args.push_back(var);

            // find the user type or interface
            Node *type = findSymbol(name);
            if (!type)
                return NULL;
            
            // if calling a constructor of an interface, emit code to bind it
            if (buildTypes->isInterface(type->getDataType())) {
                dt = type->getDataType();

                Node *scope = leftValue->getScope();
                UserType *parent= dynamic_cast<UserType*>(scope);
                const string toInjection = parent ?
                    parent->getName() + "." + leftValue->getName() :
                    leftValue->getName();

                const string injectFuncName = ":get_injection_for_" + toInjection;
                Node *funcNode = findSymbol(injectFuncName);
                FunctionBase *funcDecl = dynamic_cast<FunctionBase*>(funcNode);
                if (!funcDecl) {
                    funcDecl = new FunctionDecl(dt, injectFuncName, new FunctionParams());
                    funcDecl->setReturnIsPointer(true);
                    funcDecl->setScope(program);
                    funcDecl->generate(NULL, NULL, global_alloc);
                    program->addSymbol(funcDecl);
                }
                return Builder->CreateCall(funcDecl->getLLVMFunction());

            } else {
                // call the init constructor
                Node *fsymbol = type->findMember("init");
                assert(fsymbol && "the init method for the type must exists");
                
                FunctionBase *initfunc = dynamic_cast<FunctionBase*>(fsymbol);
                Builder->SetInsertPoint(block);

                if (initfunc->needsParent()) {
                    Type *thisTy = buildTypes->llvmType(func->getThisArgDt());
                    Value *ptr = Builder->CreateLoad(thisTy->getPointerTo(), func->getThisArg(), "derefthis");
                    args.push_back(ptr);
                }

                CallInst *c = Builder->CreateCall(initfunc->getLLVMFunction(), ArrayRef<Value*>(args));
                return NULL;
            }
        }
    }

    if (!symbol)
        symbol = ident.getSymbol(getScope(), false);

    if (symbol == NULL) {
        yyerrorcpp("Function " + name + " not defined.", this);
        return NULL;
    }

    FunctionBase *fsymbol = dynamic_cast<FunctionBase*>(symbol);
    if (fsymbol == NULL) {
        yyerrorcpp("Symbol " + name + " is not a function.", this);
        return NULL;
    } else if (ident.isComplex()) {
        Node *stem = ident.getStem().getSymbol(getScope());
        if (Interface *intf = dynamic_cast<Interface*>(stem)) {
            yyerrorcpp("Can not call an interface function.", this);
            return NULL;
        }
    }

    if (fsymbol->getNumCodedParams() != parameters->getNumParams()) {
        yyerrorcpp(string_format("Function %s has %d argument(s) but was called with %d.",
            name.c_str(), fsymbol->getNumCodedParams(), 
            parameters->getNumParams()), this);
        yywarncpp("The function declaration is here.", symbol);
        return NULL;
    }

    dt = fsymbol->getDataType();

    Node *stemSymbol = NULL;
    Value *stem = func->getThisArg();
    DataType stemdt = func->getThisArgDt();

    Builder->SetInsertPoint(block);
    if (ident.isComplex()) {
        Identifier istem = ident.getStem();
        stemSymbol = istem.getSymbol(getScope());
        stem = stemSymbol->getLLVMValue(func);
        stemdt = stemSymbol->getDataType();
        
        // TODO: When accessing a.x.func(), need to load a and gep x
        //Load loadstem(ident.getStem());
        //loadstem.setParent(this->parent);
        //stem = loadstem.generate(func, block, allocblock);
    }

    vector<Value*> args;
    for (int i = 0, j = 0; i < parameters->getNumParams(); i++, j++) {
        Node *param = parameters->getParamElement(i);
        DataType call_dt = param->getDataType();
        DataType def_dt = fsymbol->getParameters().getParamType(j);

        Value *valor = param->generate(func, block, allocblock);
        if (!valor)
            return NULL;

        if (buildTypes->isInterface(call_dt)) {
            valor = Builder->CreateLoad(valor->getType()->getPointerTo(), valor, "defer");
        } else if (buildTypes->isComplex(call_dt)) {
            //we don't support cohercion between user types yet
            if (call_dt != def_dt) {
                yyerrorcpp(string_format("Argument %s expects '%s' but '%s' was provided.",
                    fsymbol->getParameters().getParamName(i).c_str(),
                    buildTypes->name(def_dt), 
                    buildTypes->name(call_dt)), this);
                yywarncpp("The function declaration is here.", fsymbol);
            }
        } else if (buildTypes->isArrayOrMatrix(call_dt)) {
            if (call_dt != def_dt) {
                yyerrorcpp(string_format("Argument %s expects '%s' but '%s' was provided.",
                    fsymbol->getParameters().getParamName(i).c_str(),
                    buildTypes->name(def_dt), 
                    buildTypes->name(call_dt)), this);
            }

            // we pass the address of the first element
            Value *zero = ConstantInt::get(Type::getInt8Ty(global_context), 0);
            Value *indexList[2] = {zero, zero};
            Value *ptr = Builder->CreateGEP(param->getLLVMType(), valor, ArrayRef<Value*>(indexList), "gep");
            valor = ptr;
        } else {
            Type *pty = buildTypes->llvmType(def_dt);
            valor = Coercion::Convert(valor, pty, block, this);
        }
        args.push_back(valor);

        // add a size parameter after each array, or .rows and .cols for matrixes
        if (buildTypes->isArrayOrMatrix(call_dt)) {
            vector<string> params;
            if (buildTypes->isArray(call_dt))
                params.push_back(".size");
            else if (buildTypes->isMatrix(call_dt)) {
                params.push_back(".rows");
                params.push_back(".cols");
            }

            for(const string& p: params) {
                string pname = param->getName() + p;
                Load ld(pname);
                ld.setScope(func);
                valor = ld.generate(func, block, allocblock);
                valor = Coercion::Convert(valor, buildTypes->llvmType(tint32), block, this);
                args.push_back(valor);
                j++;
            }
        }
    }

    // this parameter
    if (stemSymbol) {
        if (stemSymbol->isPointerToPointer())
            stem = Builder->CreateLoad(stem->getType()->getPointerTo(), stem, "defer");
        args.push_back(stem);
    } else if (fsymbol->getThisArgDt() != BuildTypes::undefinedType) {
        // calling a function of the type itself, without stem
        Type *thisTy = buildTypes->llvmType(func->getThisArgDt());
        Value *ptr = Builder->CreateLoad(thisTy->getPointerTo(), func->getThisArg(), "derefthis");
        args.push_back(ptr);
    }

    ArrayRef<Value*> argsRef(args);

    Builder->SetInsertPoint(allocblock);
    Value *vfunc = symbol->getLLVMValue(func);
    Function *cfunc = dyn_cast<Function>(vfunc);

    // symbol->getLLVMValue above can emit another location (preGenerate of the 
    // function being called), so we emit location again
    RobDbgInfo.emitLocation(this);

    // when calling an interface function, we generate a larger function name,
    // including the type name. This enable binding the correct function implementattion
    // at the end of the build, according to the given hardware .spec
    if (stemSymbol && buildTypes->isInterface(stemSymbol->getDataType())) {
        string inject_name;
        
        if (UserType *ut = dynamic_cast<UserType*>(stemSymbol->getScope()))
            inject_name = stemSymbol->getScope()->getName() + ":";
        else if (buildTypes->isUserType(stemSymbol->getScope()->getDataType())) {
            inject_name = buildTypes->name(stemSymbol->getScope()->getDataType());
            inject_name.append(":");
        }

        inject_name.append(stemSymbol->getName() + ":");
        inject_name.append(ident.getLastName());
        Function *intf_cfunc = mainmodule->getFunction(inject_name);
        if (!intf_cfunc) {
            intf_cfunc = Function::Create(cfunc->getFunctionType(), Function::ExternalLinkage, 
                codeAddrSpace, inject_name, mainmodule);
            intf_cfunc->setDSOLocal(true);
            intf_cfunc->setCallingConv(CallingConv::C);
        }
        cfunc = intf_cfunc;
    }

    Builder->SetInsertPoint(block);
    return Builder->CreateCall(cfunc, argsRef);
}

void FunctionCall::accept(Visitor& v) {
    v.visit(*this);
}
