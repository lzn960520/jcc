#include <llvm/IR/Module.h>
#include <iostream>

#include "Context.h"
#include "exception.h"
#include "Function.h"
#include "Symbol.h"
#include "Type.h"
#include "ArgumentList.h"
#include "Identifier.h"

Context::Context() {
	currentContext = &llvm::getGlobalContext();
	module = new llvm::Module("top", *currentContext);
	builder = new llvm::IRBuilder<>(*currentContext);

	currentFunction = NULL;

	contextStack.push_back(new SymbolContext());

	std::vector<llvm::Type*> arg;

	arg.push_back(llvm::Type::getInt8PtrTy(*currentContext));
	Function *function = new Function(PUBLIC, &Type::Int32, new Identifier("puts"),
			new ArgumentList(&Type::String, new Identifier("a")),
			llvm::Function::Create(llvm::FunctionType::get(builder->getInt32Ty(), llvm::ArrayRef<llvm::Type*>(arg), false), llvm::Function::ExternalLinkage, "puts", module));
	addSymbol(new Symbol("puts", function));

	arg.clear();
	function = new Function(PUBLIC, &Type::Int32, new Identifier("readInt"),
			new ArgumentList(),
			llvm::Function::Create(llvm::FunctionType::get(builder->getInt32Ty(), llvm::ArrayRef<llvm::Type*>(arg), false), llvm::Function::ExternalLinkage, "readInt", module));
	addSymbol(new Symbol("readInt", function));

	arg.clear();
	arg.push_back(llvm::Type::getInt32Ty(*currentContext));
	function = new Function(PUBLIC, NULL, new Identifier("writeInt"),
			new ArgumentList(&Type::Int32, new Identifier("a")),
			llvm::Function::Create(llvm::FunctionType::get(builder->getVoidTy(), llvm::ArrayRef<llvm::Type*>(arg), false), llvm::Function::ExternalLinkage, "writeInt", module));
	addSymbol(new Symbol("writeInt", function));
}

llvm::Function* Context::createFunction(const std::string &name, llvm::FunctionType *funcType) {
	assert(contextStack.size() == 1);
	SymbolContext *cxt = new SymbolContext();
	contextStack.push_back(cxt);
	llvm::Function *function = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, name, module);
	llvm::BasicBlock *block = llvm::BasicBlock::Create(*currentContext, name + "@entry", function);
	builder->SetInsertPoint(block);
	currentFunction = function;
	return function;
}

void Context::endFunction() {
	assert(contextStack.size() == 2);
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
	llvm::BasicBlock *block = llvm::BasicBlock::Create(*currentContext, "", currentFunction);
	builder->SetInsertPoint(block);
	return block;
}

llvm::BasicBlock* Context::newBlock(const std::string &name) {
	llvm::BasicBlock *block = llvm::BasicBlock::Create(*currentContext, currentFunction->getName() + "@" + name, currentFunction);
	builder->SetInsertPoint(block);
	return block;
}

void Context::pushContext() {
	contextStack.push_back(new SymbolContext());
}

void Context::popContext() {
	assert(contextStack.size() > 2);
	delete contextStack.back();
	contextStack.pop_back();
}

void Context::addSymbol(Symbol *symbol) {
	assert(!contextStack.empty());
	if (contextStack.back()->count(symbol->name))
		throw Redefination(symbol->name);
	contextStack.back()->insert(std::make_pair(symbol->name, symbol));
}

Symbol* Context::findSymbol(const std::string &name) {
	assert(!contextStack.empty());
	for (SymbolContextStack::reverse_iterator it = contextStack.rbegin(); it != contextStack.rend(); it++)
		if ((*it)->count(name))
			return (**it)[name];
	return NULL;
}

std::string Context::getFunctionName() {
	if (currentFunction)
		return currentFunction->getName();
	else
		return "";
}

void Context::setBlock(llvm::BasicBlock *targetBlock) {
	assert(currentFunction);
	builder->SetInsertPoint(targetBlock);
}

llvm::BasicBlock* Context::currentBlock() {
	return builder->GetInsertBlock();
}
