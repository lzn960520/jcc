#include "yyvaltypes.h"

LiteralString::LiteralString(const char *text) :
	text(text) {
}

Json::Value LiteralString::json() {
	Json::Value root;
	root["name"] = "literal_string";
	root["text"] = Json::Value(text);
	return root;
}

LiteralString::~LiteralString() {
}

void* LiteralString::gen(Context &context) {
	return context.getBuilder().CreatePointerCast(context.getBuilder().CreateGlobalString(text), llvm::Type::getInt8PtrTy(context.getContext()));
}
