#ifndef _REPEAT_STATEMENT_H_
#define _REPEAT_STATEMENT_H_

#include "ASTNode.h"

struct RepeatStatement : public ASTNode {
	ASTNode *body, *until;
	RepeatStatement(ASTNode *body, ASTNode *until);
	~RepeatStatement();
	Json::Value json() override;
	void* gen(Context &context);
};

#endif
