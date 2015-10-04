#ifndef _IDENTIFIER_H_
#define _IDENTIFIER_H_

#include <string>

#include "ASTNode.h"

struct Identifier : public ASTNode {
	std::string text;
	Identifier(const char *name);
	~Identifier();
	Json::Value json() override;
	const std::string& getName() const;
	void* gen(Context &context) override;
};

#endif
