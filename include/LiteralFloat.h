#ifndef _LITERAL_FLOAT_H_
#define _LITERAL_FLOAT_H_

#include "Expression.h"

class LiteralFloat : public Expression {
	Type *type;
	float val;
public:
	LiteralFloat(float val);
	~LiteralFloat();
	Json::Value json() override;
	Type* getType(Context &context) override { return type; }
	llvm::Value* load(Context &context) override;
	llvm::Instruction* store(Context &context, llvm::Value *value) override;
	bool isConstant() override { return true; }
	Constant loadConstant() override;
	Type* getTypeConstant() override { return type; }
	LiteralFloat* clone() const override;
};

#endif
