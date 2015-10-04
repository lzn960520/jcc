#include <llvm/IR/Function.h>

#include "Function.h"
#include "Visibility.h"
#include "Identifier.h"
#include "Type.h"
#include "Context.h"
#include "ArgumentList.h"

Function::Function(Visibility visibility, Type *return_type, Identifier *identifier, ArgumentList *arg_list, ASTNode *body) :
	visibility(visibility), return_type(return_type), identifier(identifier), arg_list(arg_list), body(body) {
}

Function::~Function() {
	if (return_type)
		delete return_type;
	if (identifier)
		delete identifier;
	if (arg_list)
		delete arg_list;
	if (body)
		delete body;
}

Json::Value Function::json() {
	Json::Value root;
	root["name"] = "function";
	root["visibility"] = VisibilityNames[visibility];
	root["identifier"] = identifier->json();
	root["arg_list"] = arg_list->json();
	root["body"] = body->json();
	return root;
}

void* Function::gen(Context &context) {
	std::vector<llvm::Type*> arg_type;
	for (ArgumentList::ListType::iterator it = arg_list->list.begin(); it != arg_list->list.end(); it++)
		arg_type.push_back(it->first->getType(context));
	llvm::Function *function = context.createFunction(identifier->getName(), llvm::FunctionType::get(return_type->getType(context), llvm::ArrayRef<llvm::Type*>(arg_type), false));
	ArgumentList::ListType::iterator it = arg_list->list.begin();
	llvm::Function::arg_iterator it2 = function->arg_begin();
	for (; it != arg_list->list.end(); it++, it2++) {
		assert(it2 != function->arg_end());
		context.addSymbol(it->second->getName(), &(*it2));
		it2->setName(it->second->getName());
	}
	body->gen(context);
	context.endFunction();
	return function;
}
