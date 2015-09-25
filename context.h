#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include <stack>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>

class Context {
	llvm::LLVMContext *currentContext;
	std::stack<llvm::LLVMContext*> contextStack;

	llvm::Module *module;
	llvm::IRBuilder<> *builder;
public:
	Context();
	llvm::Module& getModule() { return *module; }
	llvm::IRBuilder<>& getBuilder() { return *builder; }
	llvm::LLVMContext& getContext() { return *currentContext; }
};

#endif
