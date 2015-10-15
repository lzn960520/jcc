#ifndef _VARIABLE_DEFINATION_H_
#define _VARIABLE_DEFINATION_H_

#include <list>

#include "ASTNode.h"

class Type;
class Identifier;
class Expression;
struct VariableDefination : public ASTNode {
	Type *type;
	std::list<std::pair<Identifier*, Expression*> > &list;
	typedef std::list<std::pair<Identifier*, Expression*> > List;
	typedef List::iterator iterator;
	VariableDefination(Type *type, std::list<std::pair<Identifier*, Expression*> > *list);
	~VariableDefination();
	Json::Value json() override;
	void gen(Context &context) override;
	iterator begin() { return list.begin(); }
	iterator end() { return list.end(); }
};

#endif
