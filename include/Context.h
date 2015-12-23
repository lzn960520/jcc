#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include <list>
#include <map>
#include <string>
#include <set>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/DIBuilder.h>

#include "location.h"

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
		typedef std::map<std::string, Symbol*>::const_iterator iterator;
		Symbol* find(const std::string &name);
		void add(Symbol *symbol);
		~SymbolContext();
		iterator begin() const;
		iterator end() const;
	};
private:
	typedef std::list<SymbolContext*> SymbolContextStack;
	SymbolContextStack contextStack;
	typedef std::list<llvm::DIScope*> DIScopeStack;
	DIScopeStack diScopeStack;
	std::map<std::string, Class*> classes;
	std::list<Module*> modules;
	std::map<std::string, std::string> aliases;
	std::list<std::string> raws;

	llvm::Module *module;
	llvm::IRBuilder<> *builder;
public:
	std::set<std::string> usingFiles;
	const bool isDebug;
	Context(bool debug);
	void initDWARF(const std::string &filename);
	~Context();
	llvm::DataLayout * const DL;
	llvm::DIBuilder * const DI;
	Module *currentModule;
	Class *currentClass;
	Function *currentFunction;
	llvm::Function * const mallocFunc;
	llvm::DICompileUnit *DIcu;
	llvm::DISubprogram *DIfunction;
	llvm::Module& getModule() { return *module; }
	llvm::IRBuilder<>& getBuilder() { return *builder; }
	llvm::LLVMContext& getContext() { return *llvmContext; }
	void pushContext();
	void pushContext(SymbolContext *context);
	void popContext();
	void popContext(SymbolContext *context);
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
	void pushDIScope(Location &loc);
	void pushDIScope(llvm::DIScope *scope);
	void popDIScope();
	Class* findClass(const std::string &name);
	void addClass(Class *cls);
	void addModule(Module *module);
	void addAlias(const std::string &aliasName, const std::string &fullName);
};

std::ostream& operator << (std::ostream &os, const Context::SymbolContext &context);

#endif
