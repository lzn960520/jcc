#ifndef _VARIABLE_DEFINATION_H_
#define _VARIABLE_DEFINATION_H_

#include <list>

#include "ASTNode.h"

class Type;
class VariableDefinationList;
struct VariableDefination : public ASTNode {
	Type *type;
	VariableDefinationList *list;
	VariableDefination(Type *type, VariableDefinationList *list);
	~VariableDefination();
	Json::Value json() override;
	void *gen(Context &context) override;
};

#endif
