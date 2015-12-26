#include <llvm/IR/Constants.h>

#include "LiteralInt.h"
#include "Context.h"
#include "Type.h"
#include "exception.h"

LiteralInt::LiteralInt(int val, bool isUnsigned) {
	if (isUnsigned) {
		type = &Type::UInt32;
		this->val._uint32 = val;
	} else {
		type = &Type::Int32;
		this->val._int32 = val;
	}
}

LiteralInt* LiteralInt::clone() const {
	if (type->isUnsigned)
		return new LiteralInt(this->val._uint32, true);
	else
		return new LiteralInt(this->val._int32, false);
}

Json::Value LiteralInt::json() {
	Json::Value root;
	root["name"] = "literal_int";
	root["is_unsigned"] = type->isUnsigned;
	if (type->isUnsigned)
		root["val"] = Json::Value(val._uint32);
	else
		root["val"] = Json::Value(val._int32);
	return root;
}

LiteralInt::~LiteralInt() {
	if (type)
		delete type;
}

llvm::Value* LiteralInt::load(Context &context) {
	return llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), val._uint32, type->isUnsigned);
}

llvm::Instruction* LiteralInt::store(Context &context, llvm::Value *value) {
	throw NotAssignable("literal integer");
}

Type* LiteralInt::getType(Context &context) {
	return type;
}

Expression::Constant LiteralInt::loadConstant() {
	Expression::Constant ans;
	if (type->isUnsigned)
		ans._uint64 = val._uint32;
	else
		ans._int64 = val._int32;
	return ans;
}
