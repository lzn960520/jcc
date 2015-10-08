#ifndef _WHILE_STATEMENTH_H_
#define _WHILE_STATEMENTH_H_

#include "ASTNode.h"

struct WhileStatement : public ASTNode {
	ASTNode *test, *body;
	WhileStatement(ASTNode *test, ASTNode *body);
	~WhileStatement();
	Json::Value json() override;
	void* gen(Context &context) override;
};

#endif
