#include <llvm/IR/Constants.h>

#include "LiteralFloat.h"
#include "Context.h"
#include "Type.h"
#include "exception.h"

LiteralFloat::LiteralFloat(float val) : val(val), type(&Type::Float) {
}

LiteralFloat* LiteralFloat::clone() const {
	return new LiteralFloat(val);
}

Json::Value LiteralFloat::json() {
	Json::Value root;
	root["name"] = "literal_float";
	root["val"] = Json::Value(val);
	return root;
}

LiteralFloat::~LiteralFloat() {
}

llvm::Value* LiteralFloat::load(Context &context) {
	return llvm::ConstantFP::get(context.getBuilder().getFloatTy(), val);
}

llvm::Instruction* LiteralFloat::store(Context &context, llvm::Value *value) {
	throw NotAssignable("literal float");
}

Expression::Constant LiteralFloat::loadConstant() {
	Expression::Constant ans;
	ans._double = val;
	return ans;
}
