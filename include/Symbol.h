#ifndef _SYMBOL_H_
#define _SYMBOL_H_

#include <llvm/IR/Value.h>

class Function;
class Identifier;
class Type;
struct Symbol {
	enum {
		IDENTIFIER,
		FUNCTION
	} type;
	std::string name;
	union {
		struct {
			Function *function;
			Symbol *next;
		} function;
		struct {
			Identifier *identifier;
			Type *type;
			llvm::Value *value;
		} identifier;
	} data;
public:
	Symbol(const std::string &name, Function *function);
	Symbol(const std::string &name, Identifier *identifier, Type *type, llvm::Value *value);
};

#endif
