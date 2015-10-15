#include "Identifier.h"
#include "exception.h"
#include "Context.h"
#include "Symbol.h"
#include "Type.h"
#include <iostream>

Identifier::Identifier(const char *name) :
	text(name) {
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
	if (ans->type == Symbol::IDENTIFIER) {
		if (ans->data.identifier.type->isArray())
			return ans->data.identifier.value;
		else if (ans->data.identifier.value->getType()->isPointerTy())
			return context.getBuilder().CreateLoad(ans->data.identifier.value);
		else
			return ans->data.identifier.value;
	} else
		throw NotImplemented("function pointer");
}

void Identifier::store(Context &context, llvm::Value *value) {
	Symbol *ans = context.findSymbol(text);
	if (ans == NULL)
		throw SymbolNotFound(text, loc);
	if (ans->type == Symbol::IDENTIFIER)
		context.getBuilder().CreateStore(value, ans->data.identifier.value);
	else
		throw NotAssignable("function pointer");
}

Type* Identifier::getType(Context &context) {
	Symbol *ans = context.findSymbol(text);
	if (ans == NULL)
		throw SymbolNotFound(text, loc);
	if (ans->type == Symbol::IDENTIFIER)
		return ans->data.identifier.type;
	else
		throw NotImplemented("function pointer");
}

Expression::Constant Identifier::loadConstant() {
	throw NotConstant("identifier");
}

Type* Identifier::getTypeConstant() {
	throw NotConstant("identifier");
}
