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
#include "Interface.h"

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

	vtableLLVMType = llvm::StructType::create(context.getContext(), getMangleName() + "V");
	members.push_back(llvm::PointerType::get(vtableLLVMType, 0));

	if (extends) {
		extendsClass = context.findClass(extends->getName());
		if (extendsClass == NULL)
			throw SymbolNotFound("Class '" + extends->getName() + "'");
		if (extendsClass->getMangleName()[0] == 'J')
			throw InvalidType("'" + extends->getName() + "' is an interface");
		llvm::StructType *extendsLLVM = (llvm::StructType *) extendsClass->getLLVMType();
		llvm::StructType::element_iterator it = extendsLLVM->element_begin();
		{
			// Copy vtable's format
			llvm::StructType *extendVtableType = (llvm::StructType*) (*it++)->getPointerElementType();
			for (llvm::StructType::element_iterator it = extendVtableType->element_begin(); it != extendVtableType->element_end(); it++)
				vtableType.push_back(*it);
		}

		{
			// Copy members
			for (; it != extendsLLVM->element_end(); it++)
				members.push_back(*it);
			for (Context::SymbolContext::iterator it = extendsClass->symbols.begin(); it != extendsClass->symbols.end(); it++)
				symbols.add(new Symbol(it->second));
		}

		{
			// Copy vtable lookup
			for (VtableEntryLookup::iterator it = extendsClass->vtableEntryLookup.begin(); it != extendsClass->vtableEntryLookup.end(); it++)
				vtableEntryLookup.insert(*it);
		}
	} else if (getMangleName()[0] == 'C') {
		llvm::SmallVector<llvm::Type*, 16> destructor_arg;
		destructor_arg.push_back(llvmType);
		vtableType.push_back(llvm::PointerType::get(llvm::FunctionType::get(context.getBuilder().getVoidTy(), destructor_arg, false), 0));
	}
	
	for (std::list<Identifier*>::iterator it = implements.begin(); it != implements.end(); it++) {
		// Process interface
		Class *cls = context.findClass((*it)->getName());
		if (cls == NULL)
			throw SymbolNotFound("Interface '" + (*it)->getName() + "'");
		if (cls->getMangleName()[0] != 'J')
			throw InvalidType("'" + (*it)->getName() + "' is not an interface");
		members.push_back(cls->llvmType);
		implementsType.push_back(std::pair<Interface*, size_t>((Interface *) cls, members.size() - 1));
		for (VtableEntryLookup::iterator it = cls->vtableEntryLookup.begin(); it != cls->vtableEntryLookup.end(); it++)
			for (VtableEntryList::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
				vtableEntryLookup[it->first].push_back(std::pair<size_t, size_t>(members.size() - 1, it2->second));
	}

	for (std::list<MemberNode*>::iterator it = list.begin(); it != list.end(); it++) {
		(*it)->cls = this;
		(*it)->genStruct(context);
	}

	vtableLLVMType->setBody(this->vtableType);
	((llvm::StructType *) llvmType)->setBody(members);

	{
		// create constructor declaration
		std::vector<llvm::Type*> arg_type;
		arg_type.push_back(llvm::PointerType::get(getLLVMType(), 0));
		constructor = llvm::Function::Create(
				llvm::FunctionType::get(
						context.getBuilder().getVoidTy(),
						llvm::ArrayRef<llvm::Type*>(arg_type),
						false),
				llvm::Function::ExternalLinkage,
				"_C" + getMangleName(),
				&context.getModule());
	}

	if (getMangleName()[0] == 'C') {
		// Create vtable data declaration
		vtables[0] = new llvm::GlobalVariable(context.getModule(), vtableLLVMType, false, llvm::GlobalVariable::ExternalLinkage, nullptr, "_V" + getMangleName());
		for (std::list<std::pair<Interface*, size_t> >::const_iterator it = implementsType.begin(); it != implementsType.end(); it++)
			vtables[it->second] = new llvm::GlobalVariable(context.getModule(), it->first->vtableLLVMType, false, llvm::GlobalVariable::ExternalLinkage, nullptr, "_V" + getMangleName() + "_" + it->first->getMangleName());
	}
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

	{
		// Check interface implementation
		for (std::list<std::pair<Interface*, size_t> >::const_iterator it = implementsType.begin(); it != implementsType.end(); it++) {
			if (!vtableEntry.count(it->second))
				throw CompileException("Class '" + getFullName() + "' doesn't implement '" + it->first->getFullName() + "'");
			std::vector<llvm::Function*> &entrys = vtableEntry[it->second];
			if (entrys.size() != it->first->vtableType.size())
				throw CompileException("Class '" + getFullName() + "' doesn't fully implement '" + it->first->getFullName() + "'");
			std::vector<llvm::Function*>::const_iterator it2 = entrys.begin();
			it2++;
			for (; it2 != entrys.end(); it2++)
				if (!*it2)
					throw CompileException("Class '" + getFullName() + "' doesn't fully implement '" + it->first->getFullName() + "'");
		}

		// Create vtables
		for (std::map<size_t, llvm::GlobalVariable*>::const_iterator it = vtables.begin(); it != vtables.end(); it++) {
			if (vtableEntry.count(it->first)) {
				std::vector<llvm::Function*> &entries = vtableEntry[it->first];
				llvm::StructType *vtableType = (llvm::StructType*) vtables[it->first]->getType()->getPointerElementType();
				std::vector<llvm::Constant*> tmp;
				size_t i = 0;
				for (std::vector<llvm::Function*>::const_iterator it2 = entries.begin(); it2 != entries.end(); it2++, i++) {
					if (it->first != 0 && i == 0) {
						tmp.push_back(context.getBuilder().getInt32(
								context.DL->getStructLayout(
										(llvm::StructType *) getLLVMType()
								)->getElementOffset(it->first)));
						continue;
					}
					if (*it2)
						tmp.push_back(*it2);
					else
						tmp.push_back(llvm::ConstantPointerNull::get((llvm::PointerType *) vtableType->getElementType(i)));
				}
				for (; i < vtableType->getNumElements(); i++)
					tmp.push_back(llvm::ConstantPointerNull::get((llvm::PointerType *) vtableType->getElementType(i)));
				vtables[it->first]->setInitializer(llvm::ConstantStruct::get((llvm::StructType *) vtables[it->first]->getType()->getPointerElementType(), llvm::ArrayRef<llvm::Constant*>(tmp)));
			} else {
				llvm::StructType *vtableType = (llvm::StructType *) it->second->getType()->getElementType();
				std::vector<llvm::Constant*> tmp;
				size_t i = 0;
				for (llvm::StructType::element_iterator it2 = vtableType->element_begin(); it2 != vtableType->element_end(); it2++)
					tmp.push_back(llvm::ConstantPointerNull::get((llvm::PointerType *) *it2));
				vtables[it->first]->setInitializer(llvm::ConstantStruct::get((llvm::StructType *) vtables[it->first]->getType()->getPointerElementType(), llvm::ArrayRef<llvm::Constant*>(tmp)));
			}
		}
		for (std::map<size_t, std::vector<llvm::Function*> >::const_iterator it = vtableEntry.begin(); it != vtableEntry.end(); it++) {
		}
	}
	
	{
		// Create constructor
		context.setBlock(llvm::BasicBlock::Create(context.getContext(), "", constructor));
		if (extendsClass) {
			std::vector<llvm::Value*> tmp;
			tmp.push_back(
					context.getBuilder().CreatePointerCast(
							constructor->arg_begin(),
							llvm::PointerType::get(extendsClass->getLLVMType(), 0)
					)
			);
			context.getBuilder().CreateCall(
					extendsClass->getConstructor(),
					llvm::ArrayRef<llvm::Value*>(tmp)
			);
		}
		for (std::map<size_t, llvm::GlobalVariable*>::const_iterator it = vtables.begin(); it != vtables.end(); it++) {
			context.getBuilder().CreateStore(
					it->second,
					context.getBuilder().CreateStructGEP(
							nullptr,
							constructor->arg_begin(),
							it->first));
		}
		context.getBuilder().CreateRetVoid();
		context.setBlock(NULL);
	}
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

void Class::addFunction(const std::string &signature, llvm::Function *function) {
	assert(vtableEntryLookup.count(signature) != 0);
	VtableEntryList &velist = vtableEntryLookup[signature];
	for (VtableEntryList::iterator it = velist.begin(); it != velist.end(); it++) {
		if (vtableEntry[it->first].size() <= it->second)
			vtableEntry[it->first].resize(it->second + 1);
		vtableEntry[it->first][it->second] = function;
	}
}

void Class::addFunctionStruct(const std::string &signature, Symbol *symbol) {
	if (vtableEntryLookup.count(signature) == 0) {
		symbols.add(symbol);
		switch (symbol->type) {
		case Symbol::FUNCTION:
			vtableType.push_back(llvm::PointerType::get(symbol->data.function.funcProto, 0));
			vtableEntryLookup[signature].push_back(VtableEntryPointer(0, vtableType.size() - 1));
			symbol->data.function.vtableOffset = 0;
			symbol->data.function.funcPtrOffset = vtableType.size() - 1;
			break;
		case Symbol::STATIC_FUNCTION:
			break;
		default:
			throw CompileException("Try to add something strange as function");
		}
	} else
		delete symbol;
}

llvm::Function* Class::getConstructor() {
	return constructor;
}

llvm::GlobalVariable* Class::getVtable(Interface *interface) {
	if (!interface)
		return vtables[0];
	for (std::list<std::pair<Interface*, size_t> >::const_iterator it = implementsType.begin(); it != implementsType.end(); it++)
		if (it->first == interface)
			return vtables[it->second];
	return NULL;
}

bool Class::isA(Class *a, Class *b) {
	if (a == b)
		return true;
	if (b->getMangleName()[0] == 'J') {
		// Is a implements b?
		for (std::list<std::pair<Interface*, size_t> >::const_iterator it = a->implementsType.begin(); it != a->implementsType.end(); it++)
			if (it->first == b)
				return true;
		return false;
	} else if (a->getMangleName()[0] == 'J')
		// An interface can't be a class
		return false;
	else {
		// Is a extends from b?
		while (a && a != b)
			a = a->extendsClass;
		return a != NULL;
	}
}
