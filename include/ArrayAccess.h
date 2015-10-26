#ifndef _ARRAY_ACCESS_H_
#define _ARRAY_ACCESS_H_

#include <llvm/IR/Value.h>

#include "Expression.h"

class Expression;
class ArrayAccessor;
struct ArrayAccess : public Expression {
	Expression *array;
	ArrayAccessor *accessor;
	ArrayAccess(Expression *array, ArrayAccessor *accessor);
	~ArrayAccess();
	Json::Value json() override;
	Type* getType(Context &context) override;
	llvm::Value* load(Context &context) override;
	llvm::Instruction* store(Context &context, llvm::Value *value) override;
	bool isConstant() override;
	Constant loadConstant() override;
	Type* getTypeConstant() override;
};

#endif
