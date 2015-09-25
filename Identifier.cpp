#include "yyvaltypes.h"
#include <llvm/IR/ValueSymbolTable.h>

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

std::string Identifier::getName() {
	return text;
}

void* Identifier::gen(Context &context) {
	return context.getBuilder().GetInsertBlock()->getValueSymbolTable()->lookup(text);
}
