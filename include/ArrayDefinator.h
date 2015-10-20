#ifndef _ARRAY_DEFINATOR_H_
#define _ARRAY_DEFINATOR_H_

#include <vector>
#include <utility>

#include "ASTNode.h"

class Expression;
struct ArrayDefinator : public ASTNode {
	std::vector<std::pair<Expression*, Expression*> > list;
	ArrayDefinator(Expression *lower, Expression *upper);
	void push_back(Expression *lower, Expression *upper);
	~ArrayDefinator();
	Json::Value json() override;
};

#endif
