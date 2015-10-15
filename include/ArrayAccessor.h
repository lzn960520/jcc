#ifndef _ARRAY_ACCESSOR_H_
#define _ARRAY_ACCESSOR_H_

#include <vector>

#include "ASTNode.h"

class Expression;
struct ArrayAccessor : public ASTNode {
	std::vector<Expression*> list;
	ArrayAccessor(Expression *expression);
	void push_back(Expression *expression);
	~ArrayAccessor();
	Json::Value json() override;
};

#endif
