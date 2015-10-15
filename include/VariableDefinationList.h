#ifndef _VARIABLE_DEFINATION_LIST_H_
#define _VARIABLE_DEFINATION_LIST_H_

#include <list>

#include "ASTNode.h"

class Identifier;
class Expression;
struct VariableDefinationList : public ASTNode {
	std::list<std::pair<Identifier*, Expression*> > list;
	VariableDefinationList(Identifier *identifier);
	VariableDefinationList(Identifier *identifier, Expression *init_value);
	void push_back(Identifier *identifier);
	void push_back(Identifier *identifier, Expression *init_value);
	~VariableDefinationList();
	Json::Value json() override;
};

#endif
