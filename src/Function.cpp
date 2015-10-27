#include <llvm/IR/Function.h>

#include "Function.h"
#include "Identifier.h"
#include "Type.h"
#include "Context.h"
#include "Symbol.h"
#include "Class.h"
#include "DebugInfo.h"
#include "Qualifier.h"

Function::Function(Qualifier *qualifier, Type *return_type, Identifier *identifier, std::list<std::pair<Type*, Identifier*> > *arg_list, ASTNode *body) :
	qualifier(qualifier), return_type(return_type), identifier(identifier), arg_list(*arg_list), body(body), llvmFunction(NULL), cls(NULL) {
}

Function::Function(Qualifier *qualifier, Type *return_type, Identifier *identifier, std::list<std::pair<Type*, Identifier*> > *arg_list, llvm::Function *function) :
	qualifier(qualifier), return_type(return_type), identifier(identifier), arg_list(*arg_list), body(NULL), llvmFunction(function), cls(NULL) {
}

Function::~Function() {
	if (return_type)
		delete return_type;
	delete qualifier;
	delete identifier;
	delete &arg_list;
	if (body)
		delete body;
}

Json::Value Function::json() {
	Json::Value root;
	root["name"] = "function";
	root["qualifier"] = qualifier->json();
	root["identifier"] = identifier->json();
	root["arg_list"] = Json::Value(Json::arrayValue);
	int i = 0;
	for (arg_iterator it = arg_list.begin(); it != arg_list.end(); i++, it++) {
		root["arg_list"][i] = Json::Value();
		root["arg_list"][i]["type"] = it->first->json();
		root["arg_list"][i]["identifier"] = it->second->json();
	}
	if (body)
		root["body"] = body->json();
	return root;
}

void Function::gen(Context &context) {
	std::vector<llvm::Type*> arg_type;
	if (!isStatic())
		arg_type.push_back(llvm::PointerType::get(cls->getLLVMType(), 0));
	for (arg_iterator it = arg_list.begin(); it != arg_list.end(); it++)
		arg_type.push_back(it->first->getType(context));
	llvmFunction = context.createFunction(
			cls->getPrefix() + identifier->getName(),
			llvm::FunctionType::get(
					return_type ? return_type->getType(context) : context.getBuilder().getVoidTy(),
					llvm::ArrayRef<llvm::Type*>(arg_type),
					false),
			this);
	*const_cast<llvm::DISubprogram**>(&context.DIfunction) = getDIFunction(context, this, loc);
	context.pushContext();
	context.pushDIScope(context.DIfunction);
	arg_iterator it = arg_list.begin();
	llvm::Function::arg_iterator it2 = llvmFunction->arg_begin();
	if (!isStatic()) {
		it2->setName("this");
		insertDeclareDebugInfo(context, cls->getType(), "this", &(*it2), loc, true);
		context.addSymbol(new Symbol("this", Symbol::ARGUMENT, cls->getType(), &(*it2)));
		it2++;
	}
	for (; it != arg_list.end(); it++, it2++) {
		assert(it2 != llvmFunction->arg_end());
		insertDeclareDebugInfo(context, it->first, it->second->getName(), &(*it2), it->second->loc, true);
		context.addSymbol(new Symbol(it->second->getName(), Symbol::ARGUMENT, it->first, &(*it2)));
		it2->setName(it->second->getName());
	}
	body->gen(context);
	context.popDIScope();
	context.popContext();
	*const_cast<llvm::DISubprogram**>(&context.DIfunction) = NULL;
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

const std::string Function::getMangleName() {
	return cls->getPrefix() + identifier->getName();
}

llvm::FunctionType* Function::getLLVMType(Context &context) {
	std::vector<llvm::Type*> arg_type;
	arg_type.push_back(llvm::PointerType::get(cls->getLLVMType(), 0));
	for (arg_iterator it = arg_list.begin(); it != arg_list.end(); it++)
		arg_type.push_back(it->first->getType(context));
	return llvm::FunctionType::get(
			return_type ? return_type->getType(context) : context.getBuilder().getVoidTy(),
			llvm::ArrayRef<llvm::Type*>(arg_type),
			false);
}
