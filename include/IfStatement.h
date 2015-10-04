#ifndef _IF_STATEMENT_H_
#define _IF_STATEMENT_H_

#include "ASTNode.h"

struct IfStatement : public ASTNode {
	ASTNode *test, *then_st, *else_st;
	IfStatement(ASTNode *test, ASTNode *then_st);
	IfStatement(ASTNode *test, ASTNode *then_st, ASTNode *else_st);
	~IfStatement();
	Json::Value json() override;
	void* gen(Context &context);
};

#endif
