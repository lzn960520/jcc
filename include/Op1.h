#ifndef _OP1_H_
#define _OP1_H_

#include "ASTNode.h"

struct Op1 : public ASTNode {
	ASTNode *operand;
	enum OpType {
		SELF_INC,
		SELF_DEC,
		BIT_NOT,
		LOGIC_NOT
	} op;
	Op1(ASTNode *operand, OpType op);
	static const char *OpNames[];
	~Op1();
	Json::Value json() override;
	void* gen(Context &context) override;
};

#endif
