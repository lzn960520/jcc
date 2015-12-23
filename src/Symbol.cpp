#include "Symbol.h"

Symbol::Symbol(const std::string &name, Function *function) :
	name(name), type(STATIC_FUNCTION) {
	data.static_function.function = function;
	data.static_function.next = NULL;
}

Symbol::Symbol(const std::string &name, llvm::FunctionType *funcProto, size_t vtableOffset, size_t funcPtrOffset) :
	name(name), type(FUNCTION) {
	data.function.funcProto = funcProto;
	data.function.vtableOffset = vtableOffset;
	data.function.funcPtrOffset = funcPtrOffset;
	data.function.next = NULL;
}

Symbol::Symbol(const std::string &name, SymbolType st, Type *type, llvm::Value *value) :
		name(name), type(st) {
	assert(st == LOCAL_VAR || st == ARGUMENT || st == STATIC_MEMBER_VAR);
	data.identifier.type = type;
	data.identifier.value = value;
}

Symbol::Symbol(const std::string &name, Type *type, size_t index) :
		name(name), type(MEMBER_VAR) {
	data.member.type = type;
	data.member.index = index;
}

Symbol::Symbol(const Symbol * const other) {
	type = other->type;
	switch (type) {
	case STATIC_FUNCTION:
		data.static_function.function = other->data.static_function.function;
		if (other->data.static_function.next)
			data.static_function.next = new Symbol(other->data.static_function.next);
		break;
	case FUNCTION:
		data.function.vtableOffset = other->data.function.vtableOffset;
		data.function.funcPtrOffset = other->data.function.funcPtrOffset;
		data.function.funcProto = other->data.function.funcProto;
		if (other->data.function.next)
			data.function.next = new Symbol(other->data.function.next);
		break;
	case LOCAL_VAR:
	case ARGUMENT:
	case STATIC_MEMBER_VAR:
		data.identifier.type = other->data.identifier.type;
		data.identifier.value = other->data.identifier.value;
		break;
	case MEMBER_VAR:
		data.member.type = other->data.member.type;
		data.member.index = other->data.member.index;
		break;
	}
}
