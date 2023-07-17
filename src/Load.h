#ifndef __LOAD_H__
#define __LOAD_H__
#include "Node.h"

class Load: public Node {
private:
	string ident;
	ComplexIdentifier *complexIdent;
public:
	Load(const string i): ident(i), complexIdent(nullptr) {}
	Load(ComplexIdentifier *ci): complexIdent(ci) {}

	virtual Value *generate(Function *func, BasicBlock *block, BasicBlock *allocblock) override;

	string getIdent() const { return ident; };

	virtual void accept(Visitor& v) override;

	virtual Type* getLLVMResultType(BasicBlock *block, BasicBlock *allocblock) override;

	bool isConstExpr(BasicBlock *block, BasicBlock *allocblock) override;
};
#endif
