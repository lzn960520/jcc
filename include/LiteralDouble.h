#ifndef _LITERAL_DOUBLE_H_
#define _LITERAL_DOUBLE_H_

#include "Expression.h"

class LiteralDouble : public Expression {
	Type *type;
	double val;
public:
	LiteralDouble(double val);
	~LiteralDouble();
	Json::Value json() override;
	Type* getType(Context &context) override { return type; }
	llvm::Value* load(Context &context) override;
	llvm::Instruction* store(Context &context, llvm::Value *value) override;
	bool isConstant() override { return true; }
	Constant loadConstant() override;
	Type* getTypeConstant() override { return type; }
	LiteralDouble* clone() const override;
};

#endif
