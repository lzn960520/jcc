#include "Symbol.h"
#include "Qualifier.h"

// for static function
Symbol::Symbol(const std::string &name, Qualifier *qualifier, Function *function) :
	name(name), type(STATIC_FUNCTION) {
	data.static_function.function = function;
	data.static_function.next = NULL;
	data.static_function.isPublic = qualifier->isPublic();
	data.static_function.isPrivate = qualifier->isPrivate();
	data.static_function.isProtected = qualifier->isProtected();
}

// for normal function
Symbol::Symbol(const std::string &name, Qualifier *qualifier, Function *function, llvm::FunctionType *funcProto, size_t vtableOffset, size_t funcPtrOffset) :
	name(name), type(FUNCTION) {
	data.function.funcProto = funcProto;
	data.function.vtableOffset = vtableOffset;
	data.function.funcPtrOffset = funcPtrOffset;
	data.function.next = NULL;
	data.function.function = function;
	data.function.isPublic = qualifier->isPublic();
	data.function.isPrivate = qualifier->isPrivate();
	data.function.isProtected = qualifier->isProtected();
}

// for arguments, local variable
Symbol::Symbol(const std::string &name, SymbolType st, Type *type, llvm::Value *value) :
		name(name), type(st) {
	assert(st == LOCAL_VAR || st == ARGUMENT);
	data.identifier.type = type;
	data.identifier.value = value;
}

// for static member
Symbol::Symbol(const std::string &name, Qualifier *qualifier, Type *type, llvm::Value *value) :
		name(name), type(STATIC_MEMBER_VAR) {
	data.identifier.type = type;
	data.identifier.value = value;
	data.identifier.isPublic = qualifier->isPublic();
	data.identifier.isPrivate = qualifier->isPrivate();
	data.identifier.isProtected = qualifier->isProtected();
}

// for normal member
Symbol::Symbol(const std::string &name, Qualifier *qualifier, Type *type, size_t index) :
		name(name), type(MEMBER_VAR) {
	data.member.type = type;
	data.member.index = index;
	data.member.isPublic = qualifier->isPublic();
	data.member.isPrivate = qualifier->isPrivate();
	data.member.isProtected = qualifier->isProtected();
}

// for class, interface
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

std::ostream& operator << (std::ostream &os, const Symbol &symbol) {
	os << "Symbol '" << symbol.name << "': ";
	switch (symbol.type) {
	case Symbol::MEMBER_VAR:
		os << "member variable\n";
		break;
	case Symbol::FUNCTION:
		os << "member function\n";
		break;
	case Symbol::STATIC_FUNCTION:
		os << "static function\n";
		break;
	case Symbol::STATIC_MEMBER_VAR:
		os << "static member\n";
		break;
	case Symbol::ARGUMENT:
		os << "argument\n";
		break;
	case Symbol::LOCAL_VAR:
		os << "local variable\n";
		break;
	}
	return os;
}
