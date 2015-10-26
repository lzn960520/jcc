#ifndef _NEW_H_
#define _NEW_H_

#include "Expression.h"

class Type;
class New : public Expression {
	Type *type;
public:
	New(Type *type);
	~New();
	Json::Value json();
	llvm::Value *load(Context &context) override;
	llvm::Instruction* store(Context &context, llvm::Value *value) override;
	Type* getType(Context &context) override;
	bool isConstant() override { return false; }
	Constant loadConstant() override;
	Type* getTypeConstant() override;
};

#endif
