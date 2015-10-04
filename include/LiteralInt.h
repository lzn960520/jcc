#ifndef _LITERAL_INT_H_
#define _LITERAL_INT_H_

#include "ASTNode.h"

struct LiteralInt : public ASTNode {
	int val;
	LiteralInt(int val);
	~LiteralInt();
	Json::Value json() override;
	void *gen(Context &context) override;
};

#endif
