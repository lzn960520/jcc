#include <llvm/IR/Type.h>

#include "Interface.h"
#include "MemberNode.h"
#include "Context.h"
#include "util.h"
#include "Function.h"
#include "Module.h"
#include "exception.h"

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
		Function *func = dynamic_cast<Function*>(*it);
		if (!func)
			throw CompileException("Interface can only have function declaration");
		if (func->isStatic() || !func->isPublic() || func->isPrivate() || !func->isDeclaration() || func->isProtected())
			throw CompileException("Function declaration can only be public, non-static");
		func->cls = this;
		func->genStruct(context);
	}

	vtableType->setBody(vtable);
}

const std::string Interface::getMangleName() {
	if (module)
		return "I" + module->getMangleName() + "I" + itos(getName().length()) + getName();
	else
		return "I" + itos(getName().length()) + getName();
}
