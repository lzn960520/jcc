#ifndef _FOR_STATEMENTH_H_
#define _FOR_STATEMENTH_H_

#include "ASTNode.h"

class VariableDefination;
class Expression;
class ForStatement : public ASTNode {
	VariableDefination *var;
	Expression *test;
	ASTNode *inc;
	ASTNode *body;
public:
	ForStatement(VariableDefination *var, Expression *test, ASTNode *inc, ASTNode *body);
	~ForStatement();
	Json::Value json() override;
	void gen(Context &context) override;
};

#endif
