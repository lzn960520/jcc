#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include <list>
#include <map>
#include <string>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/DIBuilder.h>

#include "yyvaltypes.h"

class Function;
class Symbol;
class Namespace;
class Class;
class Context {
	llvm::LLVMContext *llvmContext;
public:
	class SymbolContext {
		std::map<std::string, Symbol*> map;
	public:
		Symbol* find(const std::string &name);
		void add(Symbol *symbol);
		~SymbolContext();
	};
private:
	typedef std::list<SymbolContext*> SymbolContextStack;
	SymbolContextStack contextStack;
	typedef std::list<llvm::DIScope*> DIScopeStack;
	DIScopeStack diScopeStack;

	llvm::Module *module;
	llvm::IRBuilder<> *builder;

	typedef std::map<std::string, Function*> FunctionContext;
	FunctionContext functions;
public:
	llvm::DataLayout * const DL;
	llvm::DIBuilder * const DI;
	Namespace *currentNS;
	Class *currentClass;
	Function *currentFunction;
	llvm::Function * const mallocFunc;
	llvm::DICompileUnit *DIcu;
	llvm::DIFile *DIfile;
	llvm::DISubprogram *DIfunction;
	Context();
	void initDWARF(const std::string &filename);
	llvm::Module& getModule() { return *module; }
	llvm::IRBuilder<>& getBuilder() { return *builder; }
	llvm::LLVMContext& getContext() { return *llvmContext; }
	void pushContext();
	void popContext();
	SymbolContext* currentContext();
	llvm::Function* createFunction(const std::string &name, llvm::FunctionType *funcType, Function *function);
	void endFunction();
	void addSymbol(Symbol *symbol);
	Symbol* findSymbol(const std::string &name);
	void removeSymbol(const std::string &name);
	llvm::BasicBlock* newBlock();
	llvm::BasicBlock* newBlock(const std::string &name);
	std::string getFunctionName();
	void setBlock(llvm::BasicBlock *targetBlock);
	llvm::BasicBlock* currentBlock();
	llvm::DIScope* currentDIScope();
	void pushDIScope(YYLTYPE &loc);
	void pushDIScope(llvm::DIScope *scope);
	void popDIScope();
};

#endif
