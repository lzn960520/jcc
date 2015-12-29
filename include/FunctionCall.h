#ifndef _FUNCTION_CALL_H_
#define _FUNCTION_CALL_H_

#include <list>
#include <vector>

#include "Expression.h"

class Identifier;
class Expression;
class Symbol;
class Type;
class FunctionCall : public Expression {
	Identifier *identifier;
	std::list<Expression*> &arg_list;
	Expression *target;
	static Symbol* bestMatch(Symbol *symbol, const std::vector<Type*> &actual_type, Context &context);
public:
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
