#ifndef _NEW_H_
#define _NEW_H_

#include "Expression.h"

class Type;
class ArrayAccessor;
class New : public Expression {
	Type *type;
	ArrayAccessor *dim;
public:
	New(Type *type);
	New(Type *type, ArrayAccessor *dim);
	~New();
	Json::Value json() override;
	llvm::Value *load(Context &context) override;
	llvm::Instruction* store(Context &context, llvm::Value *value) override;
	Type* getType(Context &context) override;
	bool isConstant() override { return false; }
	Constant loadConstant() override;
	Type* getTypeConstant() override;
	New* clone() const override;
};

#endif
