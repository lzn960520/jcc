#include "yyvaltypes.h"
#include "exception.h"
#include <llvm/IR/Module.h>

FunctionCall::FunctionCall(ASTNode *identifier, ASTNode *arg_list) :
	identifier((Identifier *) identifier), arg_list((CallArgumentList *) arg_list) {
}

FunctionCall::~FunctionCall() {
	if (identifier)
		delete identifier;
	if (arg_list)
		delete arg_list;
}

Json::Value FunctionCall::json() {
	Json::Value root;
	root["name"] = "call";
	root["identifier"] = identifier->json();
	root["arg_list"] = arg_list->json();
	return root;
}

void* FunctionCall::gen(Context &context) {
	llvm::Function *function = context.getModule().getFunction(identifier->getName());
	if (function == NULL)
		throw FunctionNotFound(identifier->getName());
	std::vector<llvm::Value*> arg_code;
	std::list<ASTNode*> &arg_list = this->arg_list->list;
	for (std::list<ASTNode*>::iterator it = arg_list.begin(); it != arg_list.end(); it++)
		arg_code.push_back((llvm::Value *) (*it)->gen(context));
	return context.getBuilder().CreateCall(function, llvm::ArrayRef<llvm::Value*>(arg_code));
}
