#ifndef __HEADER_H__
#define __HEADER_H__

#define ENABLE_ARDUINO
#define ENABLE_PRINT

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/InlineAsm.h>

using namespace std;
using namespace llvm;

class Node;
class Stmts;
class ArrayElements;

typedef struct {
	Node *value;
	unsigned count;
} ArrayElement;

#include "bison.hpp"


extern int yyerror(const char *s);
extern int yylex();

// Program main module
extern Module *mainmodule;
extern BasicBlock *mainblock;
extern BasicBlock *global_alloc;

extern char* build_filename;
static LLVMContext global_context;

// symbol table
extern map<BasicBlock*, map<string, Value*>> tabelasym;

// arduino functions
extern Function *analogWrite;
extern Function *analogRead;
extern Function *delay;
extern Function *delayMicroseconds;

extern Function *init;
extern Function *print;
extern Function *i16div;

static int yyerrorcpp(const string& s) {
	string e = "semantic error, " + s;
	return yyerror(e.c_str());
}

static Value *search_symbol(const string& ident, BasicBlock *firstb = NULL, BasicBlock *secondb = NULL) {
	BasicBlock *blocks[] = {firstb, secondb, mainblock, global_alloc};
	for(BasicBlock *b : blocks) {
		if (b == NULL) 
			continue;
		auto blocksym = tabelasym[b];
		auto var = blocksym.find(ident);
		if (var != blocksym.end())
			return var->second;
	}
	return NULL;
}

#include "ArrayElements.h"
#include "AttachInterrupt.h"
#include "BinaryOp.h"
#include "Capsule.h"
#include "CmpOp.h"
#include "Coercion.h"
#include "Delay.h"
#include "Float.h"
#include "FunctionCall.h"
#include "FunctionDecl.h"
#include "Grid.h"
#include "If.h"
#include "Int8.h"
#include "Int16.h"
#include "Int32.h"
#include "InPort.h"
#include "Load.h"
#include "LoadVector.h"
#include "Node.h"
#include "OutPort.h"
#include "Print.h"
#include "Program.h"
#include "Return.h"
#include "Stmts.h"
#include "Scalar.h"
#include "Semantic.h"
#include "String.h"
#include "Vector.h"
#include "While.h"

#endif

