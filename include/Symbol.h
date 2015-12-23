#ifndef _SYMBOL_H_
#define _SYMBOL_H_

#include <llvm/IR/Value.h>
#include <llvm/IR/DerivedTypes.h>

class Function;
class Identifier;
class Type;
class Class;
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
		struct {
			Function *function;
			Symbol *next;
		} static_function;
		struct {
			llvm::FunctionType *funcProto;
			size_t vtableOffset, funcPtrOffset;
			Symbol *next;
		} function;
		struct {
			Type *type;
			llvm::Value *value;
		} identifier;
		struct {
			Type *type;
			size_t index;
		} member;
		struct {
			Class *cls;
		} cls;
	} data;
public:
	Symbol(const std::string &name, llvm::FunctionType *funcProto, size_t vtableOffset, size_t funcPtrOffset); // for normal function
	Symbol(const std::string &name, Function *function); // for static function
	Symbol(const std::string &name, SymbolType st, Type *type, llvm::Value *value); // for arguments, local variable, static member
	Symbol(const std::string &name, Type *type, size_t index); // for normal member
	Symbol(const std::string &name, Class *cls); // for class, interface
	Symbol(const Symbol * const other);
};

#endif
