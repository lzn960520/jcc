#ifndef _LITERAL_BOOL_H_
#define _LITERAL_BOOL_H_

#include "Expression.h"

class LiteralBool : public Expression {
	Type *type;
	bool val;
public:
	LiteralBool(bool val);
	~LiteralBool();
	Json::Value json() override;
	Type* getType(Context &context) override;
	llvm::Value* load(Context &context) override;
	llvm::Instruction* store(Context &context, llvm::Value *value) override;
	bool isConstant() override { return true; }
	Constant loadConstant() override;
	Type* getTypeConstant() override { return type; }
	LiteralBool* clone() const override;
};

#endif
