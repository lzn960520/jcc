#ifndef _CLASS_H_
#define _CLASS_H_

#include <llvm/IR/DerivedTypes.h>
#include <list>

#include "ASTNode.h"
#include "Context.h"

class Identifier;
class Namespace;
class Module;
class Type;
class Class : public ASTNode {
	Identifier *identifier;
	std::list<ASTNode*> &list;
	std::string ns, prefix;
	Type *type;
	llvm::StructType *llvmType;
	Context::SymbolContext symbols;
public:
	Class(Identifier *identifier, std::list<ASTNode*> *definations);
	~Class();
	Json::Value json() override;
	void genStruct(Context &context);
	void gen(Context &context) override;
	Type* getType() { return type; }
	llvm::Type* getLLVMType() { return llvmType; }
	void setNS(const std::string &ns);
	const std::string& getPrefix() { return prefix; }
	Symbol* findSymbol(const std::string &name);
	const std::string& getName();
};

#endif
