#ifndef _LITERAL_STRING_H_
#define _LITERAL_STRING_H_

#include <string>

#include "Expression.h"

class LiteralString : public Expression {
	std::string text;
	std::string ori_text;
public:
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
	LiteralString* clone() const override;
	const std::string& getText() const { return text; }
};

#endif
