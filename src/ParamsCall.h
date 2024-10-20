#ifndef __PARAMS_CALL_H__
#define __PARAMS_CALL_H__

#include "Node.h"

class ParamsCall {
private:
	std::vector<Node *> parameters;
public:
	ParamsCall();
	void append(Node *pc);
	void insert(int at, Node *pc);
	
	unsigned getNumParams() const;
	Node *getParamElement (int position) const;

	std::vector<Node *>& getParameters() {
		return parameters;
	}
};

#endif

