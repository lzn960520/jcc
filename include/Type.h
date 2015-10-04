#ifndef _TYPE_H_
#define _TYPE_H_

#include <llvm/IR/Type.h>

#include "ASTNode.h"

struct Type : public ASTNode {
	enum BaseType {
		BYTE,
		SHORT,
		INT,
		CHAR,
		FLOAT,
		DOUBLE
	} baseType;
	static const char *BaseTypeNames[];
	bool isUnsigned;
	Type(BaseType baseType, bool isUnsigned = false);
	~Type();
	Json::Value json() override;
	llvm::Type *getType(Context &context);
};

#endif
