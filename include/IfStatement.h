#ifndef _IF_STATEMENT_H_
#define _IF_STATEMENT_H_

#include "ASTNode.h"

class Expression;
struct IfStatement : public ASTNode {
	Expression *test;
	ASTNode *then_st, *else_st;
	IfStatement(Expression *test, ASTNode *then_st);
	IfStatement(Expression *test, ASTNode *then_st, ASTNode *else_st);
	~IfStatement();
	Json::Value json() override;
	void* gen(Context &context);
};

#endif
