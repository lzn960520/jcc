#ifndef _FUNCTION_CALL_H_
#define _FUNCTION_CALL_H_

#include "ASTNode.h"

class Identifier;
class CallArgumentList;
struct FunctionCall : public ASTNode {
	Identifier *identifier;
	CallArgumentList *arg_list;
	FunctionCall(ASTNode *identifier, ASTNode *arg_list);
	~FunctionCall();
	Json::Value json() override;
	void* gen(Context &context) override;
};

#endif
