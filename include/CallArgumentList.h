#ifndef _CALL_ARGUMENT_LIST_H_
#define _CALL_ARGUMENT_LIST_H_

#include <list>

#include "ASTNode.h"

struct CallArgumentList : public ASTNode {
	std::list<ASTNode*> list;
	CallArgumentList();
	CallArgumentList(ASTNode *expression);
	void push_back(ASTNode *expression);
	~CallArgumentList();
	Json::Value json() override;
};

#endif
