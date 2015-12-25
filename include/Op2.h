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
		LOG_XOR,
		BIT_OR_ASSIGN,
		BIT_AND_ASSIGN,
		BIT_XOR_ASSIGN,
		LOG_OR_ASSIGN,
		LOG_AND_ASSIGN,
		LOG_XOR_ASSIGN,
		ADD_ASSIGN,
		SUB_ASSIGN,
		MUL_ASSIGN,
		DIV_ASSIGN,
		MOD_ASSIGN,
		PWR_ASSIGN,
		LSH,
		RSH,
		LSH_ASSIGN,
		RSH_ASSIGN,
		BIT_OR,
		BIT_AND,
		BIT_XOR
	} op;
	Op2(Expression *left, OpType op, Expression *right);
	static const char *OpNames[];
	~Op2();
	Json::Value json() override;
	llvm::Value* load(Context &context) override;
	llvm::Instruction* store(Context &context, llvm::Value *value) override;
	Type* getType(Context &context) override;
	bool isConstant() override;
	Constant loadConstant() override;
	Type* getTypeConstant() override;
	static bool hasAssign(OpType op);
};

#endif
