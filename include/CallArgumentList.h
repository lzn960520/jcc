#ifndef _CALL_ARGUMENT_LIST_H_
#define _CALL_ARGUMENT_LIST_H_

#include <list>

#include "ASTNode.h"

class Expression;
struct CallArgumentList : public ASTNode {
	std::list<Expression*> list;
	CallArgumentList();
	CallArgumentList(Expression *expression);
	void push_back(Expression *expression);
	~CallArgumentList();
	Json::Value json() override;
};

#endif
