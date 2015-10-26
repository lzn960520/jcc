#ifndef _LITERAL_STRING_H_
#define _LITERAL_STRING_H_

#include <string>

#include "Expression.h"

struct LiteralString : public Expression {
	std::string text;
	LiteralString(const char *text);
	~LiteralString();
	Json::Value json() override;
	llvm::Value* load(Context &context) override;
	llvm::Instruction* store(Context &context, llvm::Value *value) override;
	Type* getType(Context &context) override;
	bool isConstant() override { return true; }
	Constant loadConstant() override {
		Constant ans;
		ans._string = text.c_str();
		return ans;
	}
	Type* getTypeConstant() override;
};

#endif
