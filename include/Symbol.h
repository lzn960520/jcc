#ifndef _SYMBOL_H_
#define _SYMBOL_H_

#include <llvm/IR/Value.h>

class Function;
class Identifier;
class Type;
struct Symbol {
	enum SymbolType {
		LOCAL_VAR,
		MEMBER_VAR,
		ARGUMENT,
		FUNCTION
	} type;
	std::string name;
	union {
		struct {
			Function *function;
			size_t index;
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
	} data;
public:
	Symbol(const std::string &name, Function *function, size_t index);
	Symbol(const std::string &name, SymbolType st, Type *type, llvm::Value *value);
	Symbol(const std::string &name, Type *type, size_t index);
};

#endif
