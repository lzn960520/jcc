#include <llvm/IR/Module.h>
#include <iostream>
#include <unistd.h>

#include "Context.h"
#include "exception.h"
#include "Function.h"
#include "Symbol.h"
#include "Type.h"
#include "Identifier.h"

void Context::SymbolContext::add(Symbol *symbol) {
	map[symbol->name] = symbol;
}

Symbol* Context::SymbolContext::find(const std::string &name) {
	if (map.count(name))
		return map[name];
	else
		return NULL;
}

Context::SymbolContext::~SymbolContext() {
	for (std::map<std::string, Symbol*>::iterator it = map.begin(); it != map.end(); it++)
		delete it->second;
}

Context::Context() : mallocFunc(NULL), DL(NULL), DI(NULL) {
	llvmContext = &llvm::getGlobalContext();
	module = new llvm::Module("top", getContext());
	builder = new llvm::IRBuilder<>(getContext());
	*const_cast<llvm::DataLayout**>(&DL) = new llvm::DataLayout(module);

	currentFunction = NULL;

	contextStack.push_back(new SymbolContext());

	{
		llvm::Type *tmp[] = { llvm::Type::getInt64Ty(*llvmContext) };
		*const_cast<llvm::Function **>(&mallocFunc) = llvm::Function::Create(
				llvm::FunctionType::get(
						llvm::Type::getInt8PtrTy(*llvmContext),
						llvm::ArrayRef<llvm::Type*>(tmp, 1),
						false),
				llvm::Function::ExternalLinkage,
				"malloc",
				module
		);
	}
}

void Context::initDWARF(const std::string &filename) {
	getModule().addModuleFlag(llvm::Module::Warning, "Dwarf Version", llvm::ConstantInt::get(getBuilder().getInt32Ty(), 4, false));
	getModule().addModuleFlag(llvm::Module::Warning, "Debug Info Version", llvm::ConstantInt::get(getBuilder().getInt32Ty(), 3, false));
	*const_cast<llvm::DIBuilder**>(&DI) = new llvm::DIBuilder(*module);

	// create debug info
	{
		char tmp[256];
		getcwd(tmp, sizeof(tmp));
		DIcu = DI->createCompileUnit(
				2, // language
				llvm::StringRef(filename), // filename
				llvm::StringRef(tmp), // path
				llvm::StringRef("jcc 0.0.1"), // producer
				false, // optimized?
				llvm::StringRef(""), // flags
				1); // runtime version
		DIfile = DI->createFile(llvm::StringRef(filename), llvm::StringRef(tmp));
		DI->createModule(DIfile, "top", "", "", "");
	}
}

llvm::Function* Context::createFunction(const std::string &name, llvm::FunctionType *funcType, Function *function) {
	llvm::Function *llvmFunction = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, name, module);
	llvm::BasicBlock *block = llvm::BasicBlock::Create(getContext(), name + "@entry", llvmFunction);
	builder->SetInsertPoint(block);
	currentFunction = function;
	return llvmFunction;
}

void Context::endFunction() {
	for (llvm::Function::iterator it = currentFunction->getLLVMFunction(*this)->begin(); it != currentFunction->getLLVMFunction(*this)->end(); it++)
		if ((it->hasNUsesOrMore(1) || &(*it) == &currentFunction->getLLVMFunction(*this)->getEntryBlock()) && it->getTerminator() == NULL) {
			if (currentFunction->getLLVMFunction(*this)->getReturnType()->isVoidTy()) {
				builder->SetInsertPoint(&(*it));
				builder->CreateRetVoid();
			} else
				throw NoReturn(currentFunction->getName().c_str());
		}
	currentFunction = NULL;
	builder->ClearInsertionPoint();
}

llvm::BasicBlock* Context::newBlock() {
	llvm::BasicBlock *block = llvm::BasicBlock::Create(getContext(), "", currentFunction->getLLVMFunction(*this));
	builder->SetInsertPoint(block);
	return block;
}

llvm::BasicBlock* Context::newBlock(const std::string &name) {
	llvm::BasicBlock *block = llvm::BasicBlock::Create(getContext(), currentFunction->getName() + "@" + name, currentFunction->getLLVMFunction(*this));
	builder->SetInsertPoint(block);
	return block;
}

void Context::pushContext() {
	contextStack.push_back(new SymbolContext());
}

void Context::popContext() {
	delete contextStack.back();
	contextStack.pop_back();
}

Context::SymbolContext* Context::currentContext() {
	return contextStack.back();
}

void Context::addSymbol(Symbol *symbol) {
	if (contextStack.back()->find(symbol->name))
		throw Redefination(symbol->name);
	contextStack.back()->add(symbol);
}

Symbol* Context::findSymbol(const std::string &name) {
	for (SymbolContextStack::reverse_iterator it = contextStack.rbegin(); it != contextStack.rend(); it++)
		if ((*it)->find(name))
			return (*it)->find(name);
	return NULL;
}

std::string Context::getFunctionName() {
	if (currentFunction)
		return currentFunction->getName();
	else
		return "";
}

void Context::setBlock(llvm::BasicBlock *targetBlock) {
	builder->SetInsertPoint(targetBlock);
}

llvm::BasicBlock* Context::currentBlock() {
	return builder->GetInsertBlock();
}

llvm::DIScope* Context::currentDIScope() {
	return diScopeStack.empty() ? DIfile : diScopeStack.back();
}

void Context::pushDIScope(YYLTYPE &loc) {
	if (diScopeStack.empty())
		diScopeStack.push_back(llvm::DILexicalBlock::get(getContext(), DIfile, DIfile, loc.first_line, loc.first_column));
	else
		diScopeStack.push_back(llvm::DILexicalBlock::get(getContext(), diScopeStack.back(), DIfile, loc.first_line, loc.first_column));
}

void Context::pushDIScope(llvm::DIScope *scope) {
	diScopeStack.push_back(scope);
}

void Context::popDIScope() {
	diScopeStack.pop_back();
}
