#include <llvm/IR/Module.h>

#include "Context.h"
#include "exception.h"

Context::Context() {
	currentContext = &llvm::getGlobalContext();
	module = new llvm::Module("top", *currentContext);
	builder = new llvm::IRBuilder<>(*currentContext);

	std::vector<llvm::Type*> arg;
	arg.push_back(llvm::Type::getInt8PtrTy(*currentContext));
	llvm::Function::Create(llvm::FunctionType::get(builder->getInt32Ty(), llvm::ArrayRef<llvm::Type*>(arg), false), llvm::Function::ExternalLinkage, "puts", module);
	currentFunction = NULL;
}

llvm::Function* Context::createFunction(const std::string &name, llvm::FunctionType *funcType) {
	assert(contextStack.empty());
	SymbolContext *cxt = new SymbolContext();
	contextStack.push_back(cxt);
	llvm::Function *function = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, name, module);
	llvm::BasicBlock *block = llvm::BasicBlock::Create(*currentContext, name + "@entry", function);
	builder->SetInsertPoint(block);
	currentFunction = function;
	return function;
}

void Context::endFunction() {
	assert(contextStack.size() == 1);
	builder->ClearInsertionPoint();
	for (llvm::Function::iterator it = currentFunction->begin(); it != currentFunction->end(); it++)
		if ((*it).hasOneUse() && (*it).getTerminator() == NULL)
			throw NoReturn(currentFunction->getName().str());
	currentFunction = NULL;
	contextStack.pop_back();
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
	assert(contextStack.size() > 1);
	delete contextStack.back();
	contextStack.pop_back();
}

void Context::addSymbol(const std::string &name, llvm::Value *value) {
	if (contextStack.back()->count(name))
		throw Redefination(name);
	contextStack.back()->insert(std::make_pair(name, value));
}

llvm::Value* Context::findSymbol(const std::string &name) {
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
	builder->SetInsertPoint(targetBlock);
}

llvm::BasicBlock* Context::currentBlock() {
	return builder->GetInsertBlock();
}
