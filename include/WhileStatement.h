#ifndef _WHILE_STATEMENTH_H_
#define _WHILE_STATEMENTH_H_

#include "ASTNode.h"

class Expression;
struct WhileStatement : public ASTNode {
	Expression *test;
	ASTNode *body;
	WhileStatement(Expression *test, ASTNode *body);
	~WhileStatement();
	Json::Value json() override;
	void* gen(Context &context) override;
};

#endif
