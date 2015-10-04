#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include "ASTNode.h"
#include "Visibility.h"

class Type;
class Identifier;
class ArgumentList;
struct Function : public ASTNode {
	Visibility visibility;
	Type *return_type;
	Identifier *identifier;
	ArgumentList *arg_list;
	ASTNode *body;
	Function(Visibility visibility, Type *return_type, Identifier *identifier, ArgumentList *arg_list, ASTNode *body);
	~Function();
	Json::Value json() override;
	void *gen(Context &context) override;
};

#endif
