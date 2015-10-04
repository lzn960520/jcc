#ifndef _LITERAL_STRING_H_
#define _LITERAL_STRING_H_

#include <string>

#include "ASTNode.h"

struct LiteralString : public ASTNode {
	std::string text;
	LiteralString(const char *text);
	~LiteralString();
	Json::Value json() override;
	void *gen(Context &context) override;
};

#endif
