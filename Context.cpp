#include "context.h"
#include <llvm/IR/Module.h>

Context::Context() {
	currentContext = &llvm::getGlobalContext();
	module = new llvm::Module("top", *currentContext);
	builder = new llvm::IRBuilder<>(*currentContext);

	std::vector<llvm::Type*> arg;
	arg.push_back(llvm::Type::getInt8PtrTy(*currentContext));
	llvm::Function::Create(llvm::FunctionType::get(builder->getInt32Ty(), llvm::ArrayRef<llvm::Type*>(arg), false), llvm::Function::ExternalLinkage, "puts", module);
	llvm::Function *mainFunc = llvm::Function::Create(llvm::FunctionType::get(builder->getVoidTy(), false), llvm::Function::ExternalLinkage, "main", module); // main function
	llvm::BasicBlock *topBlock = llvm::BasicBlock::Create(*currentContext, "", mainFunc);
	builder->SetInsertPoint(topBlock);
}
