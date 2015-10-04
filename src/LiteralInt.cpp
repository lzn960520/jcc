#include <llvm/IR/Constants.h>

#include "LiteralInt.h"
#include "Context.h"

LiteralInt::LiteralInt(int val) :
	val(val) {
}

Json::Value LiteralInt::json() {
	Json::Value root;
	root["name"] = "literal_int";
	root["val"] = Json::Value(val);
	return root;
}

LiteralInt::~LiteralInt() {
}

void *LiteralInt::gen(Context &context) {
	return llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), val, false);
}
