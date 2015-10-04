#ifndef _VARIABLE_DEFINATION_H_
#define _VARIABLE_DEFINATION_H_

#include <list>

#include "ASTNode.h"

class Type;
class Identifier;
struct VariableDefination : public ASTNode {
	Type *type;
	std::list<std::pair<Identifier*,ASTNode*> > list;
	VariableDefination(ASTNode *type);
	void push_back(ASTNode *identifier);
	void push_back(ASTNode *identifier, ASTNode *init_value);
	~VariableDefination();
	Json::Value json() override;
	void *gen(Context &context) override;
};

#endif
