#include "Symbol.h"

Symbol::Symbol(const std::string &name, Function *function, size_t index) :
	name(name), type(FUNCTION) {
	data.function.function = function;
	data.function.index = index;
}

Symbol::Symbol(const std::string &name, SymbolType st, Type *type, llvm::Value *value) :
		name(name), type(st) {
	assert(st == LOCAL_VAR || st == ARGUMENT);
	data.identifier.type = type;
	data.identifier.value = value;
}

Symbol::Symbol(const std::string &name, Type *type, size_t index) :
		name(name), type(MEMBER_VAR) {
	data.member.type = type;
	data.member.index = index;
}

Symbol::Symbol(const std::string &name, Class *cls) :
		name(name), type(CLASS) {
	data.cls.cls = cls;
}
