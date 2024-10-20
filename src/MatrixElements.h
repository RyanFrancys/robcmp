
#pragma once

#include "ArrayElements.h"

class MatrixElement {
public:
	ArrayElements *array;
	unsigned count;
	MatrixElement(ArrayElements *array, unsigned count): array(array), count(count) {}
};

class MatrixElements {
private:
	std::vector<MatrixElement*> elements;
	SourceLocation location;
public:
	MatrixElements(location_t loc);
	void append(MatrixElement *m);
	
	unsigned getColumnCount();
	unsigned getRowCount();
	unsigned getElementCount(int position);
	DataType getMatrixType();
	
	std::vector<MatrixElement*> const& getElements() {
		return elements;
	}
};
