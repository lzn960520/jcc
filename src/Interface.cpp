#include <llvm/IR/Type.h>

#include "Interface.h"
#include "MemberNode.h"
#include "Context.h"
#include "util.h"
#include "Function.h"
#include "Module.h"
#include "exception.h"
#include "Symbol.h"

Interface::Interface(Identifier *identifier, std::list<MemberNode*> *list) :
		Class(identifier, NULL, new std::list<Identifier*>(), list) {
}

Interface::~Interface() {
}

Json::Value Interface::json() {
	Json::Value root = Class::json();
	root["name"] = "interface";
	root.removeMember("extends");
	root.removeMember("implements");
	return root;
}

void Interface::gen(Context &context) {
}

void Interface::genStruct(Context &context) {
	llvm::StructType *vtableType = llvm::StructType::create(context.getContext(), getMangleName() + "V");
	llvmType = llvm::PointerType::get(vtableType, 0);
	context.addClass(this);

	for (std::list<MemberNode*>::iterator it = list.begin(); it != list.end(); it++) {
		Function *func = (Function *) *it;
		if (func->isStatic() || !func->isPublic() || func->isPrivate() || !func->isDeclaration() || func->isProtected())
			throw CompileException("Function declaration can only be public, non-static");
		func->cls = this;
		func->genStruct(context);
	}

	vtableType->setBody(ownVtableContent);
}

const std::string Interface::getMangleName() const {
	if (module)
		return "I" + module->getMangleName() + "I" + itos(getName().length()) + getName();
	else
		return "I" + itos(getName().length()) + getName();
}

void Interface::addFunction(const std::string &mangleName, llvm::Function *function) {
	throw CompileException("Can't add function defination to interface");
}

void Interface::addFunctionStruct(const std::string &mangleName, Symbol *symbol) {
	switch (symbol->type) {
	case Symbol::FUNCTION:
		ownVtableContent.push_back(llvm::PointerType::get(symbol->data.function.funcProto, 0));
		symbol->data.function.vtableOffset = 0;
		symbol->data.function.funcPtrOffset = ownVtableContent.size() - 1;
		symbols.add(symbol);
		break;
	default:
		throw CompileException("Try to add strange things to interface");
	}
}
