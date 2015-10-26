#ifndef _EXPRESSION_H_
#define _EXPRESSION_H_

#include <llvm/IR/Value.h>

#include "ASTNode.h"

class Type;
class Expression : public ASTNode {
public:
	typedef union {
		long long _int64;
		unsigned long long _uint64;
		double _double;
		const char *_string;
		bool _bool;
	} Constant;
	void gen(Context &context) override final { this->load(context); };
	virtual Type* getType(Context &context) = 0;
	virtual llvm::Value* load(Context &context) = 0;
	virtual llvm::Instruction* store(Context &context, llvm::Value *value) = 0;
	virtual bool isConstant() = 0;
	virtual Constant loadConstant() = 0;
	virtual Type* getTypeConstant() = 0;
};

#endif
