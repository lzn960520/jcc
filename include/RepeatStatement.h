#ifndef _REPEAT_STATEMENT_H_
#define _REPEAT_STATEMENT_H_

#include "ASTNode.h"

class Expression;
struct RepeatStatement : public ASTNode {
	ASTNode *body;
	Expression *until;
	RepeatStatement(ASTNode *body, Expression *until);
	~RepeatStatement();
	Json::Value json() override;
	void* gen(Context &context);
};

#endif
