#ifndef _FUNCTION_CALL_H_
#define _FUNCTION_CALL_H_

#include "Expression.h"

class Identifier;
class CallArgumentList;
class Expression;
struct FunctionCall : public Expression {
	Identifier *identifier;
	CallArgumentList *arg_list;
	Expression *target;
	FunctionCall(Expression *target, Identifier *identifier, CallArgumentList *arg_list);
	~FunctionCall();
	Json::Value json() override;
	llvm::Value* load(Context &context) override;
	void store(Context &context, llvm::Value *value) override;
	Type* getType(Context &context) override;
	bool isConstant() override { return false; }
	Constant loadConstant() override;
	Type* getTypeConstant() override;
};

#endif
