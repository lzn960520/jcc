#ifndef _STATEMENTS_H_
#define _STATEMENTS_H_

#include <list>

#include "ASTNode.h"

struct Statements : public ASTNode {
	std::list<ASTNode*> statements;
	Statements();
	Statements(ASTNode *statement);
	void push_back(ASTNode *statement);
	~Statements();
	Json::Value json() override;
	void* gen(Context &context) override;
};

#endif
