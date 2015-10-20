#ifndef _IDENTIFIER_H_
#define _IDENTIFIER_H_

#include <string>

#include "Expression.h"

struct Identifier : public Expression {
	std::string text;
	Identifier(const char *name);
	~Identifier();
	Json::Value json() override;
	const std::string& getName() const;
	llvm::Value* load(Context &context) override;
	void store(Context &context, llvm::Value *value) override;
	Type* getType(Context &context) override;
	bool isConstant() override { return false; }
	Constant loadConstant() override;
	Type* getTypeConstant() override;
};

#endif
