#include "HeaderGlobals.h"
#include "SourceLocation.h"
#include "Node.h"
#include "BackLLVM.h"

void DebugInfo::emitLocation(SourceLocation *s) {
	if (!debug_info || !s || s->getLineNo() == 0)
		return;
	
	DIScope *scope;
	if (scopes.empty())
		scope = cunit;
	else
		scope = scopes.back();
	
	Builder->SetCurrentDebugLocation(DILocation::get(
		scope->getContext(), s->getLineNo(), s->getColNo(), scope));
}

void DebugInfo::push_scope(DIScope *s) {
	scopes.push_back(s);
}

void DebugInfo::push_file(DIFile *f) {
	files.push_back(f);
}

void DebugInfo::pop_scope() {
	scopes.pop_back();
}

void DebugInfo::pop_file() {
	files.pop_back();
}

DIFile* DebugInfo::currFile() {
	if (!debug_info)
		return NULL;
	return files.back();
}

DIScope* DebugInfo::currScope() {
	if (!debug_info)
		return NULL;
	return scopes.back();
}

DIExpression *DebugInfo::getFixedOffsetExpression() {
	// TODO: For some unknown reason (to me), we need to offset 
	// the var declaration by one to see its actual value
	if (currentTarget().backend == rb_avr) {
		SmallVector<uint64_t, 8> Ops;
		DIExpression *ex = DBuilder->createExpression();
		ex->appendOffset(Ops, 1);
		return DIExpression::prependOpcodes(ex, Ops, false);
	} else {
		return DBuilder->createExpression();
	}
}

void DebugInfo::declareVar(Node *n, Value *alloc, BasicBlock *allocblock) {
	DataType dt = n->getDataType();
	llvm::DIType *dty = buildTypes->diType(dt);
	if (n->isPointerToPointer())
		dty = buildTypes->diPointerType(dt);
	
	if (n->hasQualifier(qvolatile)) {
		dty = DBuilder->createQualifiedType(dwarf::DW_TAG_volatile_type, dty);
	}
	auto sp = RobDbgInfo.currScope();
	auto funit = RobDbgInfo.currFile();
	DILocalVariable *d = DBuilder->createAutoVariable(
		sp, n->getName(), funit, n->getLineNo(), dty);
	
	DBuilder->insertDeclare(alloc, d, getFixedOffsetExpression(),
		DILocation::get(sp->getContext(), n->getLineNo(), n->getColNo(), sp), allocblock);
}

void DebugInfo::declareGlobalVar(Node *n, GlobalVariable *gv, BasicBlock *allocblock) {
	DataType dt = n->getDataType();
	llvm::DIType *dty = buildTypes->diType(dt);
	if (n->isPointerToPointer())
		dty = buildTypes->diPointerType(dt);
	if (n->hasQualifier(qvolatile)) {
		dty = DBuilder->createQualifiedType(dwarf::DW_TAG_volatile_type, dty);
	}
	auto sp = RobDbgInfo.currScope();
	auto funit = RobDbgInfo.currFile();
	auto *d = DBuilder->createGlobalVariableExpression(sp, gv->getName(), "",
		funit, n->getLineNo(), dty, false);
	gv->addDebugInfo(d);
}