#ifndef _VARIABLE_DEFINATION_H_
#define _VARIABLE_DEFINATION_H_

#include <list>

#include "ASTNode.h"

class Type;
class Identifier;
class Expression;
class VariableDefination : public ASTNode {
	Type *type;
	std::list<std::pair<Identifier*, Expression*> > &list;
public:
	typedef std::list<std::pair<Identifier*, Expression*> > List;
	typedef List::const_iterator iterator;
	VariableDefination(Type *type, std::list<std::pair<Identifier*, Expression*> > *list);
	~VariableDefination();
	Json::Value json() override;
	void gen(Context &context) override;
	iterator begin() { return list.begin(); }
	iterator end() { return list.end(); }
	Type* getType() const { return type; }
	size_t getSize() const { return list.size(); }
};

#endif
