#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include <list>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>

#include "MemberNode.h"
#include "Qualifier.h"

class Type;
class Identifier;
class ArgumentList;
class Class;
class Function : public MemberNode {
	Qualifier *qualifier;
	Type *return_type;
	Identifier *identifier;
	std::list<std::pair<Type*, Identifier*> > &arg_list;
	ASTNode *body;
public:
	typedef std::list<std::pair<Type*, Identifier*> >::iterator arg_iterator;
	Function(Qualifier *qualifier, Type *return_type, Identifier *identifier, std::list<std::pair<Type*, Identifier*> > *arg_list, ASTNode *body);
	~Function();
	arg_iterator arg_begin() { return arg_list.begin(); }
	arg_iterator arg_end() { return arg_list.end(); }
	Json::Value json() override;
	void gen(Context &context) override;
	llvm::Function *llvmFunction;
	llvm::Function* getLLVMFunction(Context &context);
	const std::string& getName();
	Type *getReturnType() { return return_type; }
	llvm::FunctionType* getLLVMType(Context &context);
	const std::string getMangleName(Context &context);
	inline bool isStatic() const { return qualifier->isStatic(); }
	inline bool isPublic() const { return qualifier->isPublic(); }
	inline bool isPrivate() const { return qualifier->isPrivate(); }
	inline bool isProtected() const { return qualifier->isProtected(); }
	inline bool isNative() const { return qualifier->isNative(); }
	void genStruct(Context &context) override;
	void writeJsymFile(std::ostream &os) override;
	bool isDeclaration() { return body == NULL && !isNative(); }
	const std::string getSignature(Context &context);
};

#endif
