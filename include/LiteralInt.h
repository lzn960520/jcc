#ifndef _LITERAL_INT_H_
#define _LITERAL_INT_H_

#include "Expression.h"

struct LiteralInt : public Expression {
	Type *type;
	union {
		int _int32;
		unsigned int _uint32;
	} val;
	LiteralInt(int val, bool isUnsigned);
	~LiteralInt();
	Json::Value json() override;
	Type* getType(Context &context) override;
	llvm::Value* load(Context &context) override;
	void store(Context &context, llvm::Value *value) override;
	bool isConstant() override { return true; }
	Constant loadConstant() override;
	Type* getTypeConstant() override { return type; }
};

#endif
