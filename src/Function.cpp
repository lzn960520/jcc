#include <llvm/IR/Function.h>

#include "Function.h"
#include "Visibility.h"
#include "Identifier.h"
#include "Type.h"
#include "Context.h"
#include "ArgumentList.h"
#include "Symbol.h"
#include "Class.h"

Function::Function(Visibility visibility, Type *return_type, Identifier *identifier, ArgumentList *arg_list, ASTNode *body) :
	visibility(visibility), return_type(return_type), identifier(identifier), arg_list(arg_list), body(body), llvmFunction(NULL), cls(NULL) {
}

Function::Function(Visibility visibility, Type *return_type, Identifier *identifier, ArgumentList *arg_list, llvm::Function *function) :
	visibility(visibility), return_type(return_type), identifier(identifier), arg_list(arg_list), body(NULL), llvmFunction(function), cls(NULL) {
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

void Function::gen(Context &context) {
	std::vector<llvm::Type*> arg_type;
	arg_type.push_back(llvm::PointerType::get(cls->getLLVMType(), 0));
	for (ArgumentList::ListType::iterator it = arg_list->list.begin(); it != arg_list->list.end(); it++)
		arg_type.push_back(it->first->getType(context));
	llvmFunction = context.createFunction(
			cls->getPrefix() + identifier->getName(),
			llvm::FunctionType::get(
					return_type ? return_type->getType(context) : context.getBuilder().getVoidTy(),
					llvm::ArrayRef<llvm::Type*>(arg_type),
					false)
	);
	ArgumentList::ListType::iterator it = arg_list->list.begin();
	llvm::Function::arg_iterator it2 = llvmFunction->arg_begin();
	it2->setName("this");
	context.addSymbol(new Symbol("this", Symbol::ARGUMENT, cls->getType(), &(*it2)));
	it2++;
	for (; it != arg_list->list.end(); it++, it2++) {
		assert(it2 != llvmFunction->arg_end());
		context.addSymbol(new Symbol(it->second->getName(), Symbol::ARGUMENT, it->first, &(*it2)));
		it2->setName(it->second->getName());
	}
	body->gen(context);
	context.endFunction();
}

llvm::Function* Function::getLLVMFunction(Context &context) {
	if (!llvmFunction)
		gen(context);
	return llvmFunction;
}

const std::string& Function::getName() {
	return identifier->getName();
}

llvm::FunctionType* Function::getLLVMType(Context &context) {
	std::vector<llvm::Type*> arg_type;
	arg_type.push_back(llvm::PointerType::get(cls->getLLVMType(), 0));
	for (ArgumentList::ListType::iterator it = arg_list->list.begin(); it != arg_list->list.end(); it++)
		arg_type.push_back(it->first->getType(context));
	return llvm::FunctionType::get(
			return_type ? return_type->getType(context) : context.getBuilder().getVoidTy(),
			llvm::ArrayRef<llvm::Type*>(arg_type),
			false);
}
