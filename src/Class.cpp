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
#include "exception.h"

Class::Class(Identifier *identifier, std::list<Identifier*> *implements, std::list<MemberNode*> *definations) :
	identifier(identifier), extends(NULL), implements(*implements), list(*definations), type(new Type(this)), llvmType(NULL) {
}

Class::Class(Identifier *identifier, Identifier *extends, std::list<Identifier*> *implements, std::list<MemberNode*> *definations) :
	identifier(identifier), extends(extends), implements(*implements), list(*definations), type(new Type(this)), llvmType(NULL) {
}

Class::~Class() {
	delete identifier;
	if (extends)
		delete extends;
	for (std::list<Identifier*>::iterator it = implements.begin(); it != implements.end(); it++)
		delete *it;
	delete &implements;
	for (std::list<MemberNode*>::iterator it = list.begin(); it != list.end(); it++)
		delete *it;
	delete &list;
}

Json::Value Class::json() {
	Json::Value root;
	root["name"] = "class";
	root["identifier"] = identifier->json();
	if (extends)
		root["extends"] = extends->json();
	root["implements"] = Json::Value(Json::arrayValue);
	int i = 0;
	for (std::list<Identifier*>::iterator it = implements.begin(); it != implements.end(); i++, it++)
		root["implements"][i] = (*it)->json();
	i = 0;
	root["definations"] = Json::Value(Json::arrayValue);
	for (std::list<MemberNode*>::iterator it = list.begin(); it != list.end(); i++, it++)
		root["definations"][i] = (*it)->json();
	return root;
}

void Class::genStruct(Context &context) {
	llvmType = llvm::StructType::create(context.getContext(), getMangleName());
	context.addClass(this);

	if (extends) {
		extendsClass = context.findClass(extends->getName());
		if (extendsClass == NULL)
			throw SymbolNotFound("Class '" + extends->getName() + "'");
		if (extendsClass->getMangleName()[0] == 'I')
			throw InvalidType("'" + extends->getName() + "' is an interface");
		llvm::StructType *extendsLLVM = (llvm::StructType *) extendsClass->getLLVMType();
		llvm::StructType::element_iterator it = extendsLLVM->element_begin();
		for (; it != extendsLLVM->element_end(); it++)
			members.push_back(*it);
		for (Context::SymbolContext::iterator it = extendsClass->symbols.begin(); it != extendsClass->symbols.end(); it++)
			symbols.add(new Symbol(it->second));
		for (VtableEntryLookup::iterator it = extendsClass->vtableEntryLookup.begin(); it != extendsClass->vtableEntryLookup.end(); it++)
			vtableEntryLookup.insert(*it);
	} else {
		llvm::SmallVector<llvm::Type*, 16> destructor_arg;
		destructor_arg.push_back(llvmType);
		members.push_back(llvm::PointerType::get(llvm::FunctionType::get(context.getBuilder().getVoidTy(), destructor_arg, false), 0));
	}

	llvm::StructType *vtableType = llvm::StructType::create(context.getContext(), getMangleName() + "V");
	members.push_back(llvm::PointerType::get(vtableType, 0));
	ownVtableOffset = members.size() - 1;
	
	for (std::list<Identifier*>::iterator it = implements.begin(); it != implements.end(); it++) {
		Class *cls = context.findClass((*it)->getName());
		if (cls == NULL)
			throw SymbolNotFound("Interface '" + (*it)->getName() + "'");
		if (cls->getMangleName()[0] != 'I')
			throw InvalidType("'" + (*it)->getName() + "' is not an interface");
		members.push_back(cls->llvmType);
		implementsType.push_back(std::pair<Interface*, size_t>((Interface *) cls, members.size() - 1));
	}

	for (std::list<MemberNode*>::iterator it = list.begin(); it != list.end(); it++) {
		(*it)->cls = this;
		(*it)->genStruct(context);
	}

	vtableType->setBody(ownVtableContent);
	((llvm::StructType *) llvmType)->setBody(members);
}

void Class::gen(Context &context) {
	context.currentClass = this;
	context.pushContext(&symbols);
	try {
		for (std::list<MemberNode*>::iterator it = list.begin(); it != list.end(); it++)
			(*it)->gen(context);
	} catch (...) {
		context.popContext(&symbols);
		context.currentClass = NULL;
		throw;
	}
	context.popContext(&symbols);
	context.currentClass = NULL;
}

Symbol* Class::findSymbol(const std::string &name) {
	return symbols.find(name);
}

const std::string& Class::getName() const {
	return identifier->getName();
}

const std::string Class::getFullName() const {
	if (module)
		return module->getFullName() + "::" + getName();
	else
		return getName();
}

const std::string Class::getMangleName() const {
	if (module)
		return "C" + module->getMangleName() + "C" + itos(getName().length()) + getName();
	else
		return "C" + itos(getName().length()) + getName();
}

void Class::addFunction(const std::string &mangleName, llvm::Function *function) {
	assert(vtableEntryLookup.count(mangleName) != 0);
	VtableEntryList &velist = vtableEntryLookup[mangleName];
	for (VtableEntryList::iterator it = velist.begin(); it != velist.end(); it++) {
		if (vtableEntry[it->first].size() <= it->second)
			vtableEntry[it->first].resize(it->second + 1);
		vtableEntry[it->first][it->second] = function;
	}
}

void Class::addFunctionStruct(const std::string &mangleName, Symbol *symbol) {
	if (vtableEntryLookup.count(mangleName) == 0) {
		symbols.add(symbol);
		switch (symbol->type) {
		case Symbol::FUNCTION:
			ownVtableContent.push_back(llvm::PointerType::get(symbol->data.function.funcProto, 0));
			vtableEntryLookup[mangleName].push_back(VtableEntryPointer(ownVtableOffset, ownVtableContent.size() - 1));
			symbol->data.function.vtableOffset = ownVtableOffset;
			symbol->data.function.funcPtrOffset = ownVtableContent.size() - 1;
			break;
		case Symbol::STATIC_FUNCTION:
			break;
		default:
			throw CompileException("Try to add something strange as function");
		}
	} else
		delete symbol;
}
