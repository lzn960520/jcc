#ifndef _OP2_H_
#define _OP2_H_

#include "Expression.h"

struct Op2 : public Expression {
	Expression *left, *right;
	enum OpType {
		ADD,
		SUB,
		MUL,
		DIV,
		MOD,
		PWR,
		LT,
		GT,
		LEQ,
		GEQ,
		ASSIGN,
		EQ,
		NEQ,
		LOG_AND,
		LOG_OR,
		LOG_XOR
	} op;
	Op2(Expression *left, OpType op, Expression *right);
	static const char *OpNames[];
	~Op2();
	Json::Value json() override;
	llvm::Value* load(Context &context) override;
	void store(Context &context, llvm::Value *value) override;
	Type* getType(Context &context) override;
	bool isConstant() override;
	Constant loadConstant() override;
	Type* getTypeConstant() override;
};

#endif
