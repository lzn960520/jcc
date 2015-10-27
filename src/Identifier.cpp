#include "Identifier.h"
#include "exception.h"
#include "Context.h"
#include "Symbol.h"
#include "Type.h"
#include "DebugInfo.h"
#include "Function.h"

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
	switch (ans->type) {
	case Symbol::ARGUMENT:
		return ans->data.identifier.value;
	case Symbol::LOCAL_VAR: {
		return addDebugLoc(
				context,
				context.getBuilder().CreateLoad(ans->data.identifier.value),
				loc);
	}
	case Symbol::MEMBER_VAR: {
		if (context.currentFunction->isStatic())
			throw CompileException("Access non-static member variable in static function");
		llvm::Value *tmp = addDebugLoc(
				context,
				context.getBuilder().CreateStructGEP(
						nullptr,
						context.findSymbol("this")->data.identifier.value,
						ans->data.member.index),
				loc);
		if (tmp->getType()->getPointerElementType()->isArrayTy())
			return tmp;
		else {
			return addDebugLoc(
					context,
					context.getBuilder().CreateLoad(tmp),
					loc);
		}
	}
	}
}

llvm::Instruction* Identifier::store(Context &context, llvm::Value *value) {
	Symbol *ans = context.findSymbol(text);
	if (ans == NULL)
		throw SymbolNotFound(text, loc);
	switch (ans->type) {
	case Symbol::ARGUMENT:
		return context.getBuilder().CreateStore(value, ans->data.identifier.value);
		break;
	case Symbol::LOCAL_VAR:
		return context.getBuilder().CreateStore(value, ans->data.identifier.value);
		break;
	case Symbol::MEMBER_VAR: {
		return context.getBuilder().CreateStore(
				value,
				addDebugLoc(
						context,
						context.getBuilder().CreateStructGEP(
								nullptr,
								context.findSymbol("this")->data.identifier.value,
								ans->data.member.index),
						loc));
		break;
	}
	case Symbol::FUNCTION:
		throw NotImplemented("function pointer");
	}
}

Type* Identifier::getType(Context &context) {
	Symbol *ans = context.findSymbol(text);
	if (ans == NULL)
		throw SymbolNotFound(text, loc);
	switch (ans->type) {
	case Symbol::LOCAL_VAR:
		return ans->data.identifier.type;
	case Symbol::ARGUMENT:
		return ans->data.identifier.type;
	case Symbol::MEMBER_VAR:
		return ans->data.member.type;
	case Symbol::FUNCTION:
		throw NotImplemented("function pointer");
	}
}

Expression::Constant Identifier::loadConstant() {
	throw NotConstant("identifier");
}

Type* Identifier::getTypeConstant() {
	throw NotConstant("identifier");
}
