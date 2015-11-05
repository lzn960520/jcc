#include <llvm/IR/Type.h>

#include "Class.h"
#include "Identifier.h"
#include "Context.h"
#include "VariableDefination.h"
#include "Type.h"
#include "Symbol.h"
#include "Function.h"
#include "Module.h"
#include "util.h"
#include "MemberNode.h"
#include "JsymFile.h"

Class::Class(Identifier *identifier, std::list<MemberNode*> *definations) :
	identifier(identifier), list(*definations), type(new Type(this)), llvmType(NULL) {
}

Class::~Class() {
	delete identifier;
	for (std::list<MemberNode*>::iterator it = list.begin(); it != list.end(); it++)
		delete *it;
	delete &list;
}

Json::Value Class::json() {
	Json::Value root;
	root["name"] = "class";
	root["identifier"] = identifier->json();
	root["definations"] = Json::Value(Json::arrayValue);
	int i = 0;
	for (std::list<MemberNode*>::iterator it = list.begin(); it != list.end(); i++, it++)
		root["definations"][i] = (*it)->json();
	return root;
}

void Class::genStruct(Context &context) {
	llvmType = llvm::StructType::create(context.getContext(), getMangleName());
	context.addClass(this);

	llvm::StructType *vtableType = llvm::StructType::create(context.getContext(), getMangleName() + "V");
	members.push_back(llvm::PointerType::get(vtableType, 0));

	llvm::SmallVector<llvm::Type*, 16> destructor_arg;
	destructor_arg.push_back(llvmType);
	vtable.push_back(llvm::PointerType::get(llvm::FunctionType::get(context.getBuilder().getVoidTy(), destructor_arg, false), 0));

	for (std::list<MemberNode*>::iterator it = list.begin(); it != list.end(); it++) {
		(*it)->cls = this;
		(*it)->genStruct(context);
	}
	vtableType->setBody(vtable);
	llvmType->setBody(members);
}

void Class::gen(Context &context) {
	context.currentClass = this;
	context.pushContext();
	for (std::list<MemberNode*>::iterator it = list.begin(); it != list.end(); it++)
		(*it)->gen(context);
	context.popContext();
	context.currentClass = NULL;
}

Symbol* Class::findSymbol(const std::string &name) {
	return symbols.find(name);
}

const std::string& Class::getName() {
	return identifier->getName();
}

const std::string Class::getFullName() {
	if (module)
		return module->getFullName() + "::" + getName();
	else
		return getName();
}

const std::string Class::getMangleName() {
	if (module)
		return module->getMangleName() + "C" + itos(getName().length()) + getName();
	else
		return "C" + itos(getName().length()) + getName();
}
