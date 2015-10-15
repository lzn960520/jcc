#ifndef _BLOCK_H_
#define _BLOCK_H_

#include "ASTNode.h"

struct Block : public ASTNode {
	ASTNode *body;
	Block(ASTNode *body);
	~Block();
	Json::Value json() override;
	void gen(Context &context) override;
};

#endif
