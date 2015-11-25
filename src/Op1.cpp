#include <llvm/IR/IRBuilder.h>

#include "Op1.h"
#include "Context.h"
#include "exception.h"
#include "Type.h"
#include "DebugInfo.h"

const char* Op1::OpNames[] = {
	"++",
	"--",
	"~",
	"!",
	"-"
};

Op1::Op1(Expression *operand, OpType op) :
	operand(operand), op(op) {
}

Json::Value Op1::json() {
	Json::Value root;
	root["name"] = "op1";
	root["operand"] = operand->json();
	root["op"] = OpNames[op];
	return root;
}

Op1::~Op1() {
	if (operand)
		delete operand;
}

llvm::Value* Op1::load(Context &context) {
	llvm::Value *ohs = operand->load(context);
	switch (op) {
	case SELF_INC:
		if (!operand->getType(context)->isInt())
			throw InvalidType("Self increment only allowed to integer");
		addDebugLoc(
				context,
				operand->store(context,
						addDebugLoc(
								context,
								context.getBuilder().CreateAdd(
										ohs,
										llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), 1, false)),
								loc)),
				loc);
		return ohs;
	case SELF_DEC:
		if (!operand->getType(context)->isInt())
			throw InvalidType("Self decrement only allowed to integer");
		addDebugLoc(
				context,
				operand->store(context,
						addDebugLoc(
								context,
								context.getBuilder().CreateSub(
										ohs,
										llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), 1, false)),
								loc)),
				loc);
		return ohs;
	case LOG_NOT:
		if (!operand->getType(context)->isBool())
			throw InvalidType("Logical-not only allowed to bool");
		return addDebugLoc(context, context.getBuilder().CreateNot(ohs), loc);
	case BIT_NOT:
		if (!operand->getType(context)->isInt())
			throw InvalidType("Bit-not only allowed to integer");
		return addDebugLoc(context, context.getBuilder().CreateNot(ohs), loc);
	}
}

llvm::Instruction* Op1::store(Context &context, llvm::Value *value) {
	throw NotAssignable("op1 expression");
}

Type* Op1::getType(Context &context) {
	switch (op) {
	case SELF_INC:
	case SELF_DEC:
	case LOG_NOT:
		return &Type::Bool;
	case BIT_NOT:
		return operand->getType(context);
	}
}

bool Op1::isConstant() {
	return operand->isConstant();
}

Expression::Constant Op1::loadConstant() {
	Expression::Constant ans;
	switch (op) {
	case SELF_INC:
	case SELF_DEC:
		throw NotConstant("self operator");
	case LOG_NOT:
		if (!operand->getTypeConstant()->isBool())
			throw InvalidType("Logical not is only allowed to bool");
		ans._bool = !operand->loadConstant()._bool;
		return ans;
	case BIT_NOT:
		if (!operand->getTypeConstant()->isInt())
			throw InvalidType("Bit not is only allowed to integer");
		ans._uint64 = ~operand->loadConstant()._uint64;
		return ans;
	}
}

Type* Op1::getTypeConstant() {
	switch (op) {
	case SELF_INC:
	case SELF_DEC:
	case LOG_NOT:
		return &Type::Bool;
	case BIT_NOT:
		return operand->getTypeConstant();
	}
}
