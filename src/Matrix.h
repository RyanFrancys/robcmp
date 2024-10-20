
#pragma once

#include "MatrixElements.h"
#include "Variable.h"

class ParamMatrix: public Variable {
private:
	DataType element_dt;
public:
	ParamMatrix(const string& n, string element_dt_name, location_t loc): Variable(n, loc) {
		this->element_dt = buildTypes->getType(element_dt_name, true);
		this->dt = buildTypes->getArrayType(element_dt_name, loc, 2, true);
	}
};

class Matrix: public Variable {
private:
	MatrixElements *melements;
	ArrayType *matrixType = NULL;
	DataType element_dt;
	void createDataType();
	unsigned int rows = 0;
	unsigned int cols = 0;

public:
	Matrix(const char *n, MatrixElements *me, location_t loc);

	virtual Value* generate(FunctionImpl *func, BasicBlock *block, BasicBlock *allocblock) override;

	std::vector<MatrixElement*> const& getElements() {
		return melements->getElements();
	}

	Node* accept(Visitor& v) override;

	virtual Type *getLLVMType() override;

	DataType getDataType() override;

	DataType getElementDataType() {
		return element_dt;
	}

	int getRows() const {
		return rows;
	}

	int getCols() const {
		return cols;
	}
};
