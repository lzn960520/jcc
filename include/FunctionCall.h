#ifndef _FUNCTION_CALL_H_
#define _FUNCTION_CALL_H_

#include <list>

#include "Expression.h"

class Identifier;
class Expression;
struct FunctionCall : public Expression {
	Identifier *identifier;
	std::list<Expression*> &arg_list;
	Expression *target;
	FunctionCall(Expression *target, Identifier *identifier, std::list<Expression*> *arg_list);
	~FunctionCall();
	Json::Value json() override;
	llvm::Value* load(Context &context) override;
	llvm::Instruction* store(Context &context, llvm::Value *value) override;
	Type* getType(Context &context) override;
	bool isConstant() override { return false; }
	Constant loadConstant() override;
	Type* getTypeConstant() override;
	Expression* clone() const override;
};

#endif
