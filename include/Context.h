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
class Module;
class Class;
class JsymFile;
class Module;
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
	std::map<std::string, Class*> classes;
	JsymFile *jsymFile;
	std::list<Module*> modules;

	llvm::Module *module;
	llvm::IRBuilder<> *builder;
	void initDWARF(const std::string &filename);
	void initJsymFile(const std::string &filename);
public:
	const bool isDebug;
	Context(const std::string &filename, bool debug);
	~Context();
	llvm::DataLayout * const DL;
	llvm::DIBuilder * const DI;
	Module *currentModule;
	Class *currentClass;
	Function *currentFunction;
	llvm::Function * const mallocFunc;
	llvm::DICompileUnit *DIcu;
	llvm::DIFile *DIfile;
	llvm::DISubprogram *DIfunction;
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
	Class* findClass(const std::string &name);
	void addClass(Class *cls);
	JsymFile& getJsymFile() { return *jsymFile; }
	void addModule(Module *module);
};

#endif
