#include "Identifier.h"
#include "exception.h"
#include "Context.h"

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

void* Identifier::gen(Context &context) {
	llvm::Value *ans = context.findSymbol(text);
	if (ans == NULL)
		throw SymbolNotFound(text, loc);
	return ans;
}
