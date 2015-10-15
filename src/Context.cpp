#include <llvm/IR/Module.h>
#include <iostream>

#include "Context.h"
#include "exception.h"
#include "Function.h"
#include "Symbol.h"
#include "Type.h"
#include "ArgumentList.h"
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

Context::Context() {
	llvmContext = &llvm::getGlobalContext();
	module = new llvm::Module("top", getContext());
	builder = new llvm::IRBuilder<>(getContext());

	currentFunction = NULL;
}

llvm::Function* Context::createFunction(const std::string &name, llvm::FunctionType *funcType) {
	SymbolContext *cxt = new SymbolContext();
	contextStack.push_back(cxt);
	llvm::Function *function = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, name, module);
	llvm::BasicBlock *block = llvm::BasicBlock::Create(getContext(), name + "@entry", function);
	builder->SetInsertPoint(block);
	currentFunction = function;
	return function;
}

void Context::endFunction() {
	for (llvm::Function::iterator it = currentFunction->begin(); it != currentFunction->end(); it++) {
		if ((it->hasNUsesOrMore(1) || &(*it) == &currentFunction->getEntryBlock()) && it->getTerminator() == NULL)
			if (currentFunction->getReturnType()->isVoidTy()) {
				builder->SetInsertPoint(&(*it));
				builder->CreateRetVoid();
			} else
				throw NoReturn(currentFunction->getName().str());
	}
	currentFunction = NULL;
	contextStack.pop_back();
	builder->ClearInsertionPoint();
}

llvm::BasicBlock* Context::newBlock() {
	llvm::BasicBlock *block = llvm::BasicBlock::Create(getContext(), "", currentFunction);
	builder->SetInsertPoint(block);
	return block;
}

llvm::BasicBlock* Context::newBlock(const std::string &name) {
	llvm::BasicBlock *block = llvm::BasicBlock::Create(getContext(), currentFunction->getName() + "@" + name, currentFunction);
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
