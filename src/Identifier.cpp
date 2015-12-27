#include "Identifier.h"
#include "exception.h"
#include "Context.h"
#include "Symbol.h"
#include "Type.h"
#include "DebugInfo.h"
#include "Function.h"
#include "MemberAccess.h"

Identifier::Identifier(const char *name) :
	text(name) {
}

Identifier* Identifier::clone() const {
	Identifier *clone = new Identifier(text.c_str());
	clone->loc = loc;
	return clone;
}

Json::Value Identifier::json() {
	Json::Value root;
	root["name"] = "identifier";
	root["text"] = Json::Value(text);
	return root;
}

Identifier::~Identifier() {
}

const std::string& Identifier::getName() const {
	return text;
}

llvm::Value* Identifier::load(Context &context) {
	Symbol *ans = context.findSymbol(text);
	if (ans == NULL)
		throw SymbolNotFound(text, loc);
	switch (ans->type) {
	case Symbol::ARGUMENT:
		return ans->data.identifier.value;
	case Symbol::LOCAL_VAR: {
		return addDebugLoc(
				context,
				context.getBuilder().CreateLoad(ans->data.identifier.value),
				loc);
	}
	case Symbol::STATIC_MEMBER_VAR: {
		MemberAccess *access = new MemberAccess(new Type(context.currentClass), clone());
		llvm::Value *ans = access->load(context);
		delete access;
		return ans; }
	case Symbol::MEMBER_VAR: {
		MemberAccess *access = new MemberAccess(new Identifier("this"), clone());
		llvm::Value *ans = access->load(context);
		delete access;
		return ans; }
	case Symbol::FUNCTION:
	case Symbol::STATIC_FUNCTION:
		throw NotImplemented("function pointer");
	}
}

llvm::Instruction* Identifier::store(Context &context, llvm::Value *value) {
	Symbol *ans = context.findSymbol(text);
	if (ans == NULL)
		throw SymbolNotFound(text, loc);
	switch (ans->type) {
	case Symbol::ARGUMENT:
		return context.getBuilder().CreateStore(value, ans->data.identifier.value);
	case Symbol::LOCAL_VAR:
		return context.getBuilder().CreateStore(value, ans->data.identifier.value);
	case Symbol::STATIC_MEMBER_VAR: {
		MemberAccess *access = new MemberAccess(new Type(context.currentClass), clone());
		llvm::Instruction *store = access->store(context, value);
		delete access;
		return store; }
	case Symbol::MEMBER_VAR: {
		MemberAccess *access = new MemberAccess(new Identifier("this"), clone());
		llvm::Instruction *store = access->store(context, value);
		delete access;
		return store; }
	case Symbol::STATIC_FUNCTION:
	case Symbol::FUNCTION:
		throw NotAssignable("function pointer");
	}
}

Type* Identifier::getType(Context &context) {
	Symbol *ans = context.findSymbol(text);
	if (ans == NULL)
		throw SymbolNotFound(text, loc);
	switch (ans->type) {
	case Symbol::LOCAL_VAR:
	case Symbol::ARGUMENT:
	case Symbol::STATIC_MEMBER_VAR:
		return ans->data.identifier.type;
	case Symbol::MEMBER_VAR:
		return ans->data.member.type;
	case Symbol::FUNCTION:
	case Symbol::STATIC_FUNCTION:
		throw NotImplemented("function pointer");
	}
}

Expression::Constant Identifier::loadConstant() {
	throw NotConstant("identifier");
}

Type* Identifier::getTypeConstant() {
	throw NotConstant("identifier");
}
