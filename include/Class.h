#ifndef _CLASS_H_
#define _CLASS_H_

#include <llvm/IR/DerivedTypes.h>
#include <list>
#include <vector>

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
	std::list<std::pair<Interface*, int> > implementsType;
	Identifier *extends;
	Class *extendsClass;
	Type *type;
	Context::SymbolContext symbols;
	std::vector<llvm::Type*> members;
	friend class Function;
	friend class MemberVariableDefination;
	friend class Type;
protected:
	std::list<MemberNode*> &list;
	llvm::Type *llvmType;
	std::vector<llvm::Type*> vtable;
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
	const std::string& getName();
	const std::string getFullName();
	virtual const std::string getMangleName();
	void writeJsymFile(std::ostream &os) override;
};

#endif
