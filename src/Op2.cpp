#include <llvm/IR/IRBuilder.h>

#include "Op2.h"
#include "Context.h"
#include "exception.h"
#include "Type.h"
#include "Expression.h"
#include "DebugInfo.h"

const char* Op2::OpNames[] = {
	"+",
	"-",
	"*",
	"/",
	"%",
	"**",
	"<",
	">",
	"<=",
	">=",
	"=",
	"==",
	"!=",
	"&&",
	"||",
	"^^",
	"|=",
	"&=",
	"^=",
	"||=",
	"&&=",
	"^^=",
	"+=",
	"-=",
	"*=",
	"/=",
	"%=",
	"**=",
	"<<",
	">>",
	"<<=",
	">>=",
	"|",
	"&",
	"^"
};

Op2::Op2(Expression *left, OpType op, Expression *right) :
	left(left), right(right), op(op) {
}

Json::Value Op2::json() {
	Json::Value root;
	root["name"] = "op2";
	root["left"] = left->json();
	root["right"] = right->json();
	root["op"] = OpNames[op];
	return root;
}

Op2::~Op2() {
	if (left)
		delete left;
	if (right)
		delete right;
}

llvm::Value* Op2::load(Context &context) {
	if (op == ASSIGN) {
		llvm::Value *tmp = right->load(context);
		addDebugLoc(
				context,
				left->store(context, tmp),
				loc);
		return tmp;
	}
	Type *ansType = Type::higherType(left->getType(context), right->getType(context));
	llvm::Value *lhs = Type::cast(context, left->getType(context), left->load(context), ansType);
	llvm::Value *rhs = Type::cast(context, right->getType(context), right->load(context), ansType);
	switch (op) {
	case ADD:
		return addDebugLoc(context, context.getBuilder().CreateAdd(lhs, rhs), loc);
	case SUB:
		return addDebugLoc(context, context.getBuilder().CreateSub(lhs, rhs), loc);
	case MUL:
		return addDebugLoc(context, context.getBuilder().CreateMul(lhs, rhs), loc);
	case DIV:
		return addDebugLoc(context, context.getBuilder().CreateSDiv(lhs, rhs), loc);
	case LT:
		if (ansType->isFloat())
			return addDebugLoc(context, context.getBuilder().CreateFCmpOLT(lhs, rhs), loc);
		else if (ansType->isInt()) {
			if (ansType->isUnsigned)
				return addDebugLoc(context, context.getBuilder().CreateICmpULT(lhs, rhs), loc);
			else
				return addDebugLoc(context, context.getBuilder().CreateICmpSLT(lhs, rhs), loc);
		} else if (ansType->isString())
			throw NotImplemented("comparison of string");
		break;
	case LEQ:
		if (ansType->isFloat())
			return addDebugLoc(context, context.getBuilder().CreateFCmpOLE(lhs, rhs), loc);
		else if (ansType->isInt()) {
			if (ansType->isUnsigned)
				return addDebugLoc(context, context.getBuilder().CreateICmpULE(lhs, rhs), loc);
			else
				return addDebugLoc(context, context.getBuilder().CreateICmpSLE(lhs, rhs), loc);
		} else if (ansType->isString())
			throw NotImplemented("comparison of string");
		break;
	case GT:
		if (ansType->isFloat())
			return addDebugLoc(context, context.getBuilder().CreateFCmpOGT(lhs, rhs), loc);
		else if (ansType->isInt()) {
			if (ansType->isUnsigned)
				return addDebugLoc(context, context.getBuilder().CreateICmpUGT(lhs, rhs), loc);
			else
				return addDebugLoc(context, context.getBuilder().CreateICmpSGT(lhs, rhs), loc);
		} else if (ansType->isString())
			throw NotImplemented("comparison of string");
		break;
	case GEQ:
		if (ansType->isFloat())
			return addDebugLoc(context, context.getBuilder().CreateFCmpOGE(lhs, rhs), loc);
		else if (ansType->isInt()) {
			if (ansType->isUnsigned)
				return addDebugLoc(context, context.getBuilder().CreateICmpUGE(lhs, rhs), loc);
			else
				return addDebugLoc(context, context.getBuilder().CreateICmpSGE(lhs, rhs), loc);
		} else if (ansType->isString())
			throw NotImplemented("comparison of string");
		break;
	case ASSIGN:
		break;
	case EQ:
		if (ansType->isFloat())
			return addDebugLoc(context, context.getBuilder().CreateFCmpOEQ(lhs, rhs), loc);
		else if (ansType->isInt())
			return addDebugLoc(context, context.getBuilder().CreateICmpEQ(lhs, rhs), loc);
		else if (ansType->isString())
			throw NotImplemented("comparison of string");
		break;
	case NEQ:
		if (ansType->isFloat())
			return addDebugLoc(context, context.getBuilder().CreateFCmpONE(lhs, rhs), loc);
		else if (ansType->isInt())
			return addDebugLoc(context, context.getBuilder().CreateICmpNE(lhs, rhs), loc);
		else if (ansType->isString())
			throw NotImplemented("comparison of string");
		break;
	case LOG_AND:
		if (!left->getType(context)->isBool() || !right->getType(context)->isBool())
			throw InvalidType("Logical and only allowed to bool");
		return context.getBuilder().CreateAnd(lhs, rhs);
	case LOG_OR:
		if (!left->getType(context)->isBool() || !right->getType(context)->isBool())
			throw InvalidType("Logical or only allowed to bool");
		return context.getBuilder().CreateOr(lhs, rhs);
	case LOG_XOR:
		if (!left->getType(context)->isBool() || !right->getType(context)->isBool())
			throw InvalidType("Logical xor only allowed to bool");
		return context.getBuilder().CreateXor(lhs, rhs);
	default:
		throw NotImplemented(std::string("operator ") + OpNames[op] + " not implemented");
	}
	throw InvalidType(std::string("can't apply operator ") + OpNames[op] + " to " + left->getType(context)->getName() + " and " + right->getType(context)->getName());
}

llvm::Instruction* Op2::store(Context &context, llvm::Value *value) {
	throw NotImplemented("store to op2");
}

Type* Op2::getType(Context &context) {
	switch (op) {
	case ADD:
	case SUB:
	case MUL:
	case DIV:
		return Type::higherType(left->getType(context), right->getType(context));
	case LT:
	case GT:
	case LEQ:
	case GEQ:
	case EQ:
	case NEQ:
	case LOG_AND:
	case LOG_OR:
	case LOG_XOR:
		return &Type::Bool;
	default:
		throw NotImplemented(std::string("operator ") + OpNames[op] + " not implemented");
	}
}

bool Op2::isConstant() {
	return left->isConstant() && right->isConstant();
}

Expression::Constant Op2::loadConstant() {
	Type* higherType = Type::higherType(left->getTypeConstant(), right->getTypeConstant());
	Expression::Constant ans;
	if (higherType->isInt())
		if (higherType->isUnsigned)
			switch (op) {
			case ADD:
				if (left->getTypeConstant()->isUnsigned)
					ans._uint64 = left->loadConstant()._uint64;
				else
					ans._uint64 = left->loadConstant()._int64;
				if (right->getTypeConstant()->isUnsigned)
					ans._uint64 += right->loadConstant()._uint64;
				else
					ans._uint64 += right->loadConstant()._int64;
				return ans;
			case SUB:
				if (left->getTypeConstant()->isUnsigned)
					ans._uint64 = left->loadConstant()._uint64;
				else
					ans._uint64 = left->loadConstant()._int64;
				if (right->getTypeConstant()->isUnsigned)
					ans._uint64 -= right->loadConstant()._uint64;
				else
					ans._uint64 -= right->loadConstant()._int64;
				return ans;
			case MUL:
				if (left->getTypeConstant()->isUnsigned)
					ans._uint64 = left->loadConstant()._uint64;
				else
					ans._uint64 = left->loadConstant()._int64;
				if (right->getTypeConstant()->isUnsigned)
					ans._uint64 *= right->loadConstant()._uint64;
				else
					ans._uint64 *= right->loadConstant()._int64;
				return ans;
			case DIV:
				if (left->getTypeConstant()->isUnsigned)
					ans._uint64 = left->loadConstant()._uint64;
				else
					ans._uint64 = left->loadConstant()._int64;
				if (right->getTypeConstant()->isUnsigned)
					ans._uint64 /= right->loadConstant()._uint64;
				else
					ans._uint64 /= right->loadConstant()._int64;
				return ans;
			case LT:
				if (left->getTypeConstant()->isUnsigned)
					ans._uint64 = left->loadConstant()._uint64;
				else
					ans._uint64 = left->loadConstant()._int64;
				if (right->getTypeConstant()->isUnsigned)
					ans._bool = ans._uint64 < right->loadConstant()._uint64;
				else
					ans._bool = ans._uint64 < right->loadConstant()._int64;
				return ans;
			case LEQ:
				if (left->getTypeConstant()->isUnsigned)
					ans._uint64 = left->loadConstant()._uint64;
				else
					ans._uint64 = left->loadConstant()._int64;
				if (right->getTypeConstant()->isUnsigned)
					ans._bool = ans._uint64 <= right->loadConstant()._uint64;
				else
					ans._bool = ans._uint64 <= right->loadConstant()._int64;
				return ans;
			case GT:
				if (left->getTypeConstant()->isUnsigned)
					ans._uint64 = left->loadConstant()._uint64;
				else
					ans._uint64 = left->loadConstant()._int64;
				if (right->getTypeConstant()->isUnsigned)
					ans._bool = ans._uint64 > right->loadConstant()._uint64;
				else
					ans._bool = ans._uint64 > right->loadConstant()._int64;
				return ans;
			case GEQ:
				if (left->getTypeConstant()->isUnsigned)
					ans._uint64 = left->loadConstant()._uint64;
				else
					ans._uint64 = left->loadConstant()._int64;
				if (right->getTypeConstant()->isUnsigned)
					ans._bool = ans._uint64 >= right->loadConstant()._uint64;
				else
					ans._bool = ans._uint64 >= right->loadConstant()._int64;
				return ans;
			default:
				throw NotImplemented(std::string("operator ") + OpNames[op] + " not implemented");
			}
		else
			switch (op) {
			case ADD:
				if (left->getTypeConstant()->isUnsigned)
					ans._int64 = left->loadConstant()._uint64;
				else
					ans._int64 = left->loadConstant()._int64;
				if (right->getTypeConstant()->isUnsigned)
					ans._int64 += right->loadConstant()._uint64;
				else
					ans._int64 += right->loadConstant()._int64;
				return ans;
			case SUB:
				if (left->getTypeConstant()->isUnsigned)
					ans._int64 = left->loadConstant()._uint64;
				else
					ans._int64 = left->loadConstant()._int64;
				if (right->getTypeConstant()->isUnsigned)
					ans._int64 -= right->loadConstant()._uint64;
				else
					ans._int64 -= right->loadConstant()._int64;
				return ans;
			case MUL:
				if (left->getTypeConstant()->isUnsigned)
					ans._int64 = left->loadConstant()._uint64;
				else
					ans._int64 = left->loadConstant()._int64;
				if (right->getTypeConstant()->isUnsigned)
					ans._int64 *= right->loadConstant()._uint64;
				else
					ans._int64 *= right->loadConstant()._int64;
				return ans;
			case DIV:
				if (left->getTypeConstant()->isUnsigned)
					ans._int64 = left->loadConstant()._uint64;
				else
					ans._int64 = left->loadConstant()._int64;
				if (right->getTypeConstant()->isUnsigned)
					ans._int64 /= right->loadConstant()._uint64;
				else
					ans._int64 /= right->loadConstant()._int64;
				return ans;
			case LT:
				if (left->getTypeConstant()->isUnsigned)
					ans._int64 = left->loadConstant()._uint64;
				else
					ans._int64 = left->loadConstant()._int64;
				if (right->getTypeConstant()->isUnsigned)
					ans._bool = ans._int64 < right->loadConstant()._uint64;
				else
					ans._bool = ans._int64 < right->loadConstant()._int64;
				return ans;
			case LEQ:
				if (left->getTypeConstant()->isUnsigned)
					ans._int64 = left->loadConstant()._uint64;
				else
					ans._int64 = left->loadConstant()._int64;
				if (right->getTypeConstant()->isUnsigned)
					ans._bool = ans._int64 <= right->loadConstant()._uint64;
				else
					ans._bool = ans._int64 <= right->loadConstant()._int64;
				return ans;
			case GT:
				if (left->getTypeConstant()->isUnsigned)
					ans._int64 = left->loadConstant()._uint64;
				else
					ans._int64 = left->loadConstant()._int64;
				if (right->getTypeConstant()->isUnsigned)
					ans._bool = ans._int64 > right->loadConstant()._uint64;
				else
					ans._bool = ans._int64 > right->loadConstant()._int64;
				return ans;
			case GEQ:
				if (left->getTypeConstant()->isUnsigned)
					ans._int64 = left->loadConstant()._uint64;
				else
					ans._int64 = left->loadConstant()._int64;
				if (right->getTypeConstant()->isUnsigned)
					ans._bool = ans._int64 >= right->loadConstant()._uint64;
				else
					ans._bool = ans._int64 >= right->loadConstant()._int64;
				return ans;
			default:
				throw NotImplemented(std::string("operator ") + OpNames[op] + " not implemented");
			}
	else if (higherType->isFloat())
		switch (op) {
		case ADD:
			if (left->getTypeConstant()->isInt())
				if (left->getTypeConstant()->isUnsigned)
					ans._double = left->loadConstant()._uint64;
				else
					ans._double = left->loadConstant()._int64;
			else
				ans._double = left->loadConstant()._double;
			if (right->getTypeConstant()->isFloat())
				if (right->getTypeConstant()->isUnsigned)
					ans._double += right->loadConstant()._uint64;
				else
					ans._double += right->loadConstant()._int64;
			else
				ans._double += right->loadConstant()._double;
			return ans;
		case SUB:
			if (left->getTypeConstant()->isInt())
				if (left->getTypeConstant()->isUnsigned)
					ans._double = left->loadConstant()._uint64;
				else
					ans._double = left->loadConstant()._int64;
			else
				ans._double = left->loadConstant()._double;
			if (right->getTypeConstant()->isFloat())
				if (right->getTypeConstant()->isUnsigned)
					ans._double -= right->loadConstant()._uint64;
				else
					ans._double -= right->loadConstant()._int64;
			else
				ans._double -= right->loadConstant()._double;
			return ans;
		case MUL:
			if (left->getTypeConstant()->isInt())
				if (left->getTypeConstant()->isUnsigned)
					ans._double = left->loadConstant()._uint64;
				else
					ans._double = left->loadConstant()._int64;
			else
				ans._double = left->loadConstant()._double;
			if (right->getTypeConstant()->isFloat())
				if (right->getTypeConstant()->isUnsigned)
					ans._double *= right->loadConstant()._uint64;
				else
					ans._double *= right->loadConstant()._int64;
			else
				ans._double *= right->loadConstant()._double;
			return ans;
		case DIV:
			if (left->getTypeConstant()->isInt())
				if (left->getTypeConstant()->isUnsigned)
					ans._double = left->loadConstant()._uint64;
				else
					ans._double = left->loadConstant()._int64;
			else
				ans._double = left->loadConstant()._double;
			if (right->getTypeConstant()->isFloat())
				if (right->getTypeConstant()->isUnsigned)
					ans._double /= right->loadConstant()._uint64;
				else
					ans._double /= right->loadConstant()._int64;
			else
				ans._double /= right->loadConstant()._double;
			return ans;
		case LT:
			if (left->getTypeConstant()->isInt())
				if (left->getTypeConstant()->isUnsigned)
					ans._double = left->loadConstant()._uint64;
				else
					ans._double = left->loadConstant()._int64;
			else
				ans._double = left->loadConstant()._double;
			if (right->getTypeConstant()->isInt())
				if (right->getTypeConstant()->isUnsigned)
					ans._bool = ans._double < right->loadConstant()._uint64;
				else
					ans._bool = ans._double < right->loadConstant()._int64;
			else
				ans._bool = ans._double < right->loadConstant()._double;
			return ans;
		case LEQ:
			if (left->getTypeConstant()->isInt())
				if (left->getTypeConstant()->isUnsigned)
					ans._double = left->loadConstant()._uint64;
				else
					ans._double = left->loadConstant()._int64;
			else
				ans._double = left->loadConstant()._double;
			if (right->getTypeConstant()->isInt())
				if (right->getTypeConstant()->isUnsigned)
					ans._bool = ans._double <= right->loadConstant()._uint64;
				else
					ans._bool = ans._double <= right->loadConstant()._int64;
			else
				ans._bool = ans._double <= right->loadConstant()._double;
			return ans;
		case GT:
			if (left->getTypeConstant()->isInt())
				if (left->getTypeConstant()->isUnsigned)
					ans._double = left->loadConstant()._uint64;
				else
					ans._double = left->loadConstant()._int64;
			else
				ans._double = left->loadConstant()._double;
			if (right->getTypeConstant()->isInt())
				if (right->getTypeConstant()->isUnsigned)
					ans._bool = ans._double > right->loadConstant()._uint64;
				else
					ans._bool = ans._double > right->loadConstant()._int64;
			else
				ans._bool = ans._double > right->loadConstant()._double;
			return ans;
		case GEQ:
			if (left->getTypeConstant()->isInt())
				if (left->getTypeConstant()->isUnsigned)
					ans._double = left->loadConstant()._uint64;
				else
					ans._double = left->loadConstant()._int64;
			else
				ans._double = left->loadConstant()._double;
			if (right->getTypeConstant()->isInt())
				if (right->getTypeConstant()->isUnsigned)
					ans._bool = ans._double >= right->loadConstant()._uint64;
				else
					ans._bool = ans._double >= right->loadConstant()._int64;
			else
				ans._bool = ans._double >= right->loadConstant()._double;
			return ans;
		default:
			throw NotImplemented(std::string("operator ") + OpNames[op] + " not implemented");
		}
	else if (higherType->isString())
		throw NotImplemented("const string calculate");
	throw NotImplemented(std::string("Const caculation ") + left->getTypeConstant()->getName() +
			OpNames[op] + right->getTypeConstant()->getName());
}

Type* Op2::getTypeConstant() {
	switch (op) {
	case ADD:
	case SUB:
	case MUL:
	case DIV:
		return Type::higherType(left->getTypeConstant(), right->getTypeConstant());
	case LT:
	case GT:
	case LEQ:
	case GEQ:
	case EQ:
	case NEQ:
	case LOG_AND:
	case LOG_OR:
	case LOG_XOR:
		return &Type::Bool;
	default:
		throw NotImplemented(std::string("operator ") + OpNames[op] + " not implemented");
	}
}
