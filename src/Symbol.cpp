#include "Symbol.h"

Symbol::Symbol(const std::string &name, Function *function) :
	name(name), type(FUNCTION) {
	data.function.function = function;
}

Symbol::Symbol(const std::string &name, Identifier *identifier, Type *type, llvm::Value *value) :
		name(name), type(IDENTIFIER) {
	data.identifier.identifier = identifier;
	data.identifier.type = type;
	data.identifier.value = value;
}
