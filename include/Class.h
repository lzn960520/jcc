#ifndef _CLASS_H_
#define _CLASS_H_

#include <llvm/IR/DerivedTypes.h>
#include <list>
#include <vector>
#include <ostream>

#include "StructNode.h"
#include "Context.h"

class Identifier;
class Namespace;
class Module;
class Type;
class MemberNode;
class Function;
class MemberVariableDefination;
class Interface;
class Class : public StructNode {
	Identifier *identifier;
	std::list<Identifier*> &implements;
	std::list<std::pair<Interface*, size_t> > implementsType;
	Identifier *extends;
	Class *extendsClass;
	Type *type;
	std::vector<llvm::Type*> members;
	typedef std::pair<size_t, size_t> VtableEntryPointer;
	typedef std::list<VtableEntryPointer> VtableEntryList;
	typedef std::map<std::string, VtableEntryList> VtableEntryLookup;
	VtableEntryLookup vtableEntryLookup;
	std::map<size_t, std::vector<llvm::Function*> > vtableEntry;
	std::map<size_t, llvm::GlobalVariable*> vtables;
	llvm::Function *constructor;
	friend class Function;
	friend class MemberVariableDefination;
	friend class Type;
protected:
	Context::SymbolContext symbols;
	std::list<MemberNode*> &list;
	llvm::Type *llvmType;
	std::vector<llvm::Type*> vtableType;
	virtual void addFunction(const std::string &mangleName, llvm::Function *function);
	virtual void addFunctionStruct(const std::string &mangleName, Symbol *symbol);
public:
	Class(Identifier *identifier, std::list<Identifier*> *implements, std::list<MemberNode*> *definations);
	Class(Identifier *identifier, Identifier *extends, std::list<Identifier*> *implements, std::list<MemberNode*> *definations);
	~Class();
	Json::Value json() override;
	void genStruct(Context &context) override;
	void gen(Context &context) override;
	Type* getType() { return type; }
	llvm::Type* getLLVMType() { return llvmType; }
	Symbol* findSymbol(const std::string &name);
	const std::string& getName() const;
	const std::string getFullName() const;
	virtual const std::string getMangleName() const;
	void writeJsymFile(std::ostream &os) override;
	llvm::Function *getConstructor();
	llvm::GlobalVariable *getVtable(Interface *interface);
	friend std::ostream& operator << (std::ostream &os, const Class &cls);
};

inline std::ostream& operator << (std::ostream &os, const Class &cls) {
	os << "Class: " << cls.getName() << std::endl;
	os << "Full name: " << cls.getFullName() << std::endl;
	os << "Mangle name: " << cls.getMangleName() << std::endl;
	os << cls.symbols;
	return os;
}

#endif
