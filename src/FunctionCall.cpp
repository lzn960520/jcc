#include "FunctionCall.h"
#include "exception.h"
#include "Context.h"
#include "Identifier.h"
#include "CallArgumentList.h"
#include "Function.h"
#include "Symbol.h"

FunctionCall::FunctionCall(Identifier *identifier, CallArgumentList *arg_list) :
	identifier(identifier), arg_list(arg_list) {
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

llvm::Value* FunctionCall::load(Context &context) {
	Symbol *symbol = context.findSymbol(identifier->getName());
	if (symbol->type != Symbol::FUNCTION)
		throw InvalidType("calling a symbol which is not a function");
	llvm::Function *function = symbol->data.function.function->getLLVMFunction(context);
	if (function == NULL)
		throw FunctionNotFound(identifier->getName());
	std::vector<llvm::Value*> arg_code;
	std::list<Expression*> &arg_list = this->arg_list->list;
	for (std::list<Expression*>::iterator it = arg_list.begin(); it != arg_list.end(); it++)
		arg_code.push_back((*it)->load(context));
	return context.getBuilder().CreateCall(function, llvm::ArrayRef<llvm::Value*>(arg_code));
}

void FunctionCall::store(Context &context, llvm::Value *value) {
	throw NotAssignable("function call");
}

Type* FunctionCall::getType(Context &context) {
	Symbol *symbol = context.findSymbol(identifier->getName());
	if (symbol->type != Symbol::FUNCTION)
		throw InvalidType("calling a symbol which is not a function");
	return symbol->data.function.function->getReturnType();
}

Expression::Constant FunctionCall::loadConstant() {
	throw NotConstant("function call");
}

Type* FunctionCall::getTypeConstant() {
	throw NotConstant("function call");
}
