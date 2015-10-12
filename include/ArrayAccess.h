#ifndef _ARRAY_ACCESS_H_
#define _ARRAY_ACCESS_H_

#include "ASTNode.h"

struct ArrayAccess : public ASTNode {
	ASTNode *array, *accessor;
	ArrayAccess(ASTNode *array, ASTNode *accessor);
	~ArrayAccess();
	Json::Value json() override;
	void* gen(Context &context) override;
};

#endif
