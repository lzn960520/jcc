#ifndef _SYMBOL_H_
#define _SYMBOL_H_

#include <llvm/IR/Value.h>
#include <llvm/IR/DerivedTypes.h>
#include <ostream>

class Function;
class Identifier;
class Type;
class Class;
class Qualifier;
struct Symbol {
	enum SymbolType {
		LOCAL_VAR,
		MEMBER_VAR,
		ARGUMENT,
		FUNCTION,
		STATIC_MEMBER_VAR,
		STATIC_FUNCTION
	} type;
	std::string name;
	union {
		// static function
		struct {
			Function *function;
			Symbol *next;
			bool isPublic, isPrivate, isProtected;
		} static_function;
		// normal function
		struct {
			Function *function;
			llvm::FunctionType *funcProto;
			size_t vtableOffset, funcPtrOffset;
			Symbol *next;
			bool isPublic, isPrivate, isProtected;
		} function;
		// local variable, argument, static member
		struct {
			Type *type;
			llvm::Value *value;
			bool isPublic, isPrivate, isProtected;
		} identifier;
		// normal member
		struct {
			Type *type;
			size_t index;
			bool isPublic, isPrivate, isProtected;
		} member;
		// class, interface
		struct {
			Class *cls;
		} cls;
	} data;
public:
	Symbol(const std::string &name, Qualifier *qualifier, Function *function, llvm::FunctionType *funcProto, size_t vtableOffset, size_t funcPtrOffset); // for normal function
	Symbol(const std::string &name, Qualifier *qualifier, Function *function); // for static function
	Symbol(const std::string &name, SymbolType st, Type *type, llvm::Value *value); // for arguments, local variable
	Symbol(const std::string &name, Qualifier *qualifier, Type *type, llvm::Value *value); // for static member
	Symbol(const std::string &name, Qualifier *qualifier, Type *type, size_t index); // for normal member
	Symbol(const std::string &name, Class *cls); // for class, interface
	Symbol(const Symbol * const other); // copy from another symbol, will also copy the whole overload function list
	friend std::ostream& operator << (std::ostream &os, const Symbol &symbol);
};

std::ostream& operator << (std::ostream &os, const Symbol &symbol);

#endif
