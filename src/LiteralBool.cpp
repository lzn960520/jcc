#include <llvm/IR/Constants.h>

#include "LiteralBool.h"
#include "Context.h"
#include "Type.h"
#include "exception.h"

LiteralBool::LiteralBool(bool val) : val(val) {
}

LiteralBool* LiteralBool::clone() const {
	return new LiteralBool(val);
}

LiteralBool::~LiteralBool() {
}

Json::Value LiteralBool::json() {
	Json::Value root;
	root["name"] = "literal_bool";
	root["val"] = val;
	return root;
}

llvm::Value* LiteralBool::load(Context &context) {
	return llvm::ConstantInt::get(context.getBuilder().getInt1Ty(), val, false);
}

llvm::Instruction* LiteralBool::store(Context &context, llvm::Value *value) {
	throw NotAssignable("literal bool");
}

Type* LiteralBool::getType(Context &context) {
	return &Type::Bool;
}

Expression::Constant LiteralBool::loadConstant() {
	Expression::Constant ans;
	ans._bool = val;
	return ans;
}
