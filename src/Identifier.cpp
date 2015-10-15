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
	switch (ans->type) {
	case Symbol::ARGUMENT:
		return ans->data.identifier.value;
	case Symbol::LOCAL_VAR:
		return context.getBuilder().CreateLoad(ans->data.identifier.value);
	case Symbol::MEMBER_VAR: {
		llvm::Value *index[2];
		index[0] = llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), 0, false);
		index[1] = llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), ans->data.member.index, false);
		llvm::Value *tmp = llvm::GetElementPtrInst::Create(
				context.findSymbol("this")->data.identifier.value,
				llvm::ArrayRef<llvm::Value*>(index, 2),
				"",
				context.currentBlock()
		);
		if (tmp->getType()->getPointerElementType()->isArrayTy())
			return tmp;
		else
			return context.getBuilder().CreateLoad(tmp);
	}
	}
}

void Identifier::store(Context &context, llvm::Value *value) {
	Symbol *ans = context.findSymbol(text);
	if (ans == NULL)
		throw SymbolNotFound(text, loc);
	switch (ans->type) {
	case Symbol::ARGUMENT:
		context.getBuilder().CreateStore(value, ans->data.identifier.value);
		break;
	case Symbol::LOCAL_VAR:
		context.getBuilder().CreateStore(value, ans->data.identifier.value);
		break;
	case Symbol::MEMBER_VAR: {
		llvm::Value *index = llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), ans->data.member.index, false);
		context.getBuilder().CreateStore(
				value,
				llvm::GetElementPtrInst::Create(
						context.findSymbol("this")->data.identifier.value,
						llvm::ArrayRef<llvm::Value*>(&index, 1),
						"",
						context.currentBlock()
				)
		);
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
