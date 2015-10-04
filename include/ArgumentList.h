#ifndef _ARGUMENT_LIST_H_
#define _ARGUMENT_LIST_H_

#include <list>

#include "ASTNode.h"

class Type;
class Identifier;
struct ArgumentList : public ASTNode {
	typedef std::list<std::pair<Type*, Identifier*> > ListType;
	ListType list;
	ArgumentList();
	ArgumentList(Type *type, Identifier *identifier);
	void push_back(Type *type, Identifier *identifier);
	~ArgumentList();
	Json::Value json() override;
};

#endif
