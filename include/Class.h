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
class Class : public StructNode {
	Identifier *identifier;
	std::list<MemberNode*> &list;
	Type *type;
	llvm::StructType *llvmType;
	Context::SymbolContext symbols;
	std::vector<llvm::Type*> members;
	std::vector<llvm::Type*> vtable;
	friend class Function;
	friend class MemberVariableDefination;
public:
	Module *module;
	Class(Identifier *identifier, std::list<MemberNode*> *definations);
	~Class();
	Json::Value json() override;
	void genStruct(Context &context) override;
	void gen(Context &context) override;
	Type* getType() { return type; }
	llvm::Type* getLLVMType() { return llvmType; }
	Symbol* findSymbol(const std::string &name);
	const std::string& getName();
	const std::string getFullName();
	const std::string getMangleName();
	void writeJsymFile(FILE *f) override;
};

#endif
