#ifndef _OP1_H_
#define _OP1_H_

#include "Expression.h"

struct Op1 : public Expression {
	Expression *operand;
	enum OpType {
		SELF_INC,
		SELF_DEC,
		BIT_NOT,
		LOGIC_NOT
	} op;
	Op1(Expression *operand, OpType op);
	static const char *OpNames[];
	~Op1();
	Json::Value json() override;
	llvm::Value* load(Context &context) override;
	llvm::Instruction* store(Context &context, llvm::Value *value) override;
	Type* getType(Context &context) override;
	bool isConstant() override;
	Constant loadConstant() override;
	Type* getTypeConstant() override;
};

#endif
