#ifndef _OP2_H_
#define _OP2_H_

#include "ASTNode.h"

struct Op2 : public ASTNode {
	ASTNode *left, *right;
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
		ASSIGN
	} op;
	Op2(ASTNode *left, OpType op, ASTNode *right);
	static const char *OpNames[];
	~Op2();
	Json::Value json() override;
	void* gen(Context &context) override;
};

#endif
