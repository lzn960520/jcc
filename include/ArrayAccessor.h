#ifndef _ARRAY_ACCESSOR_H_
#define _ARRAY_ACCESSOR_H_

#include <list>

#include "ASTNode.h"

struct ArrayAccessor : public ASTNode {
	std::list<ASTNode*> list;
	ArrayAccessor(ASTNode *expression);
	void push_back(ASTNode *expression);
	~ArrayAccessor();
	Json::Value json() override;
};

#endif
