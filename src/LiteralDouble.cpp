#include <llvm/IR/Constants.h>

#include "LiteralDouble.h"
#include "Context.h"
#include "Type.h"
#include "exception.h"

LiteralDouble::LiteralDouble(double val) : val(val), type(&Type::Double) {
}

LiteralDouble* LiteralDouble::clone() const {
	return new LiteralDouble(val);
}

Json::Value LiteralDouble::json() {
	Json::Value root;
	root["name"] = "literal_double";
	root["val"] = Json::Value(val);
	return root;
}

LiteralDouble::~LiteralDouble() {
}

llvm::Value* LiteralDouble::load(Context &context) {
	return llvm::ConstantFP::get(context.getBuilder().getDoubleTy(), val);
}

llvm::Instruction* LiteralDouble::store(Context &context, llvm::Value *value) {
	throw NotAssignable("literal double");
}

Expression::Constant LiteralDouble::loadConstant() {
	Expression::Constant ans;
	ans._double = val;
	return ans;
}
