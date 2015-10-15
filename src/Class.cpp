#include <llvm/IR/Type.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/DIBuilder.h>

#include "Class.h"
#include "Identifier.h"
#include "Context.h"
#include "Namespace.h"
#include "VariableDefination.h"
#include "Type.h"
#include "Symbol.h"
#include "Function.h"

Class::Class(Identifier *identifier, std::list<ASTNode*> *definations) :
	identifier(identifier), list(*definations), type(new Type(this)), llvmType(NULL) {
}

Class::~Class() {
	delete identifier;
	for (std::list<ASTNode*>::iterator it = list.begin(); it != list.end(); it++)
		delete *it;
	delete &list;
}

Json::Value Class::json() {
	Json::Value root;
	root["name"] = "class";
	root["identifier"] = identifier->json();
	root["definations"] = Json::Value(Json::arrayValue);
	int i = 0;
	for (std::list<ASTNode*>::iterator it = list.begin(); it != list.end(); i++, it++)
		root["definations"][i] = (*it)->json();
	return root;
}

void Class::genStruct(Context &context) {
	llvmType = llvm::StructType::create(context.getContext(), ns + identifier->getName());
	std::vector<llvm::Type*> members;
	int i = 1;
	members.push_back(llvm::PointerType::get(llvm::Type::getInt8PtrTy(context.getContext()), 0));
	for (std::list<ASTNode*>::iterator it = list.begin(); it != list.end(); it++) {
		if (typeid(**it) == typeid(VariableDefination)) {
			VariableDefination *def = (VariableDefination *) *it;
			for (VariableDefination::iterator it = def->begin(); it != def->end(); it++) {
				members.push_back(def->type->getType(context));
				symbols.add(new Symbol(it->first->getName(), def->type, i));
				i++;
			}
		} else if (typeid(**it) == typeid(Function)) {
			Function *def = (Function *) *it;
			def->cls = this;
			members.push_back(llvm::PointerType::get(def->getLLVMType(context), 0));
			symbols.add(new Symbol(def->getName(), def, i));
			i++;
		}
	}
	llvmType->setBody(members);
}

void Class::gen(Context &context) {
	context.pushContext();
	int i = 1;
	for (std::list<ASTNode*>::iterator it = list.begin(); it != list.end(); it++) {
		if (typeid(**it) == typeid(VariableDefination)) {
			VariableDefination *def = (VariableDefination *) *it;
			for (VariableDefination::iterator it = def->begin(); it != def->end(); it++)
				context.addSymbol(new Symbol(it->first->getName(), def->type, i++));
		}
	}
	for (std::list<ASTNode*>::iterator it = list.begin(); it != list.end(); it++) {
		if (typeid(**it) == typeid(Function)) {
			Function *def = (Function *) *it;
			def->gen(context);
		}
	}
	context.popContext();
}

void Class::setNS(const std::string &ns) {
	this->ns = ns;
	prefix = ns + identifier->getName() + "$";
}

Symbol* Class::findSymbol(const std::string &name) {
	return symbols.find(name);
}
