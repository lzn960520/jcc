#ifndef _MEMBER_ACCESS_H_
#define _MEMBER_ACCESS_H_

#include "Expression.h"

class Identifier;
class Class;
class MemberAccess : public Expression {
	Expression *target;
	Identifier *identifier;
	bool isStatic;
	Type *targetClass;
public:
	MemberAccess(Expression *target, Identifier *identifier);
	MemberAccess(Type *type, Identifier *identifier);
	~MemberAccess();
	Json::Value json() override;
	llvm::Value* load(Context &context) override;
	llvm::Instruction* store(Context &context, llvm::Value *value) override;
	Type* getType(Context &context) override;
	Type* getTypeConstant() override;
	bool isConstant() override { return false; }
	Constant loadConstant() override;
};

#endif
