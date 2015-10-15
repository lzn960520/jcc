#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include <list>
#include <map>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>

class Function;
class Symbol;
class Context {
	llvm::LLVMContext *currentContext;
	typedef std::map<std::string, Symbol*> SymbolContext;
	typedef std::list<SymbolContext*> SymbolContextStack;
	SymbolContextStack contextStack;

	llvm::Module *module;
	llvm::IRBuilder<> *builder;
	llvm::Function *currentFunction;

	typedef std::map<std::string, Function*> FunctionContext;
	FunctionContext functions;
public:
	Context();
	llvm::Module& getModule() { return *module; }
	llvm::IRBuilder<>& getBuilder() { return *builder; }
	llvm::LLVMContext& getContext() { return *currentContext; }
	void pushContext();
	void popContext();
	llvm::Function* createFunction(const std::string &name, llvm::FunctionType *funcType);
	void endFunction();
	void addSymbol(Symbol *symbol);
	Symbol* findSymbol(const std::string &name);
	void removeSymbol(const std::string &name);
	llvm::BasicBlock* newBlock();
	llvm::BasicBlock* newBlock(const std::string &name);
	std::string getFunctionName();
	void setBlock(llvm::BasicBlock *targetBlock);
	llvm::BasicBlock* currentBlock();
};

#endif
