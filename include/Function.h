#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include <llvm/IR/DerivedTypes.h>

#include "ASTNode.h"
#include "Visibility.h"

class Type;
class Identifier;
class ArgumentList;
class Class;
struct Function : public ASTNode {
	Visibility visibility;
	Type *return_type;
	Identifier *identifier;
	ArgumentList *arg_list;
	ASTNode *body;
	Class *cls;
	Function(Visibility visibility, Type *return_type, Identifier *identifier, ArgumentList *arg_list, ASTNode *body);
	Function(Visibility visibility, Type *return_type, Identifier *identifier, ArgumentList *arg_list, llvm::Function *llvmFunction);
	~Function();
	Json::Value json() override;
	void gen(Context &context) override;
	llvm::Function *llvmFunction;
	llvm::Function* getLLVMFunction(Context &context);
	const std::string& getName();
	Type *getReturnType() { return return_type; }
	llvm::FunctionType* getLLVMType(Context &context);
};

#endif
