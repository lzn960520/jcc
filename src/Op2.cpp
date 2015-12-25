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
		llvm::Value *tmp = Type::cast(context, right->getType(context), right->load(context), left->getType(context));
		addDebugLoc(
				context,
				left->store(context, tmp),
				loc);
		return tmp;
	}
	Type *ansType = Type::higherType(left->getType(context), right->getType(context));
	llvm::Value *lhs = Type::cast(context, left->getType(context), left->load(context), ansType);
	llvm::Value *rhs = Type::cast(context, right->getType(context), right->load(context), ansType);

	// pre type check
	switch (op) {
	case ADD:
	case ADD_ASSIGN:
	case SUB:
	case SUB_ASSIGN:
	case MUL:
	case MUL_ASSIGN:
	case DIV:
	case DIV_ASSIGN:
	case PWR:
	case PWR_ASSIGN:
	case MOD:
	case MOD_ASSIGN:
	case LT:
	case GT:
	case LEQ:
	case GEQ:
	case EQ:
	case NEQ:
		if (!ansType->isNumber() && !ansType->isString())
			throw InvalidType(std::string("can't apply operator ") + OpNames[op] + " to " + left->getType(context)->getName() + " and " + right->getType(context)->getName());
		break;
	case LSH:
	case LSH_ASSIGN:
	case RSH:
	case RSH_ASSIGN:
	case BIT_OR:
	case BIT_OR_ASSIGN:
	case BIT_AND:
	case BIT_AND_ASSIGN:
	case BIT_XOR:
	case BIT_XOR_ASSIGN:
		if (!ansType->isInt())
			throw InvalidType(std::string("can't apply operator ") + OpNames[op] + " to " + left->getType(context)->getName() + " and " + right->getType(context)->getName());
		break;
	case LOG_OR:
	case LOG_OR_ASSIGN:
	case LOG_AND:
	case LOG_AND_ASSIGN:
	case LOG_XOR:
	case LOG_XOR_ASSIGN:
		if (!ansType->isBool())
			throw InvalidType(std::string("can't apply operator ") + OpNames[op] + " to " + left->getType(context)->getName() + " and " + right->getType(context)->getName());
		break;
	case ASSIGN:
		// this has been handled before
		break;
	}

	switch (op) {
	case ADD_ASSIGN:
	case ADD: {
		llvm::Value *tmp;
		if (ansType->isFloat())
			tmp = addDebugLoc(context, context.getBuilder().CreateFAdd(lhs, rhs), loc);
		else
			tmp = addDebugLoc(context, context.getBuilder().CreateAdd(lhs, rhs), loc);
		if (hasAssign(op)) {
			tmp = Type::cast(context, ansType, tmp, left->getType(context));
			addDebugLoc(
					context,
					left->store(context, tmp),
					loc);
		}
		return tmp; }
	case SUB_ASSIGN:
	case SUB: {
		llvm::Value *tmp;
		if (ansType->isFloat())
			tmp = addDebugLoc(context, context.getBuilder().CreateFSub(lhs, rhs), loc);
		else
			tmp = addDebugLoc(context, context.getBuilder().CreateSub(lhs, rhs), loc);
		if (hasAssign(op)) {
			tmp = Type::cast(context, ansType, tmp, left->getType(context));
			addDebugLoc(
					context,
					left->store(context, tmp),
					loc);
		}
		return tmp; }
	case MUL_ASSIGN:
	case MUL: {
		llvm::Value *tmp;
		if (ansType->isFloat())
			tmp = addDebugLoc(context, context.getBuilder().CreateFMul(lhs, rhs), loc);
		else
			tmp = addDebugLoc(context, context.getBuilder().CreateMul(lhs, rhs), loc);
		if (hasAssign(op)) {
			tmp = Type::cast(context, ansType, tmp, left->getType(context));
			addDebugLoc(
					context,
					left->store(context, tmp),
					loc);
		}
		return tmp; }
	case DIV_ASSIGN:
	case DIV: {
		llvm::Value *tmp;
		if (ansType->isFloat())
			tmp = addDebugLoc(context, context.getBuilder().CreateFDiv(lhs, rhs), loc);
		else if (ansType->isUnsigned)
			tmp = addDebugLoc(context, context.getBuilder().CreateUDiv(lhs, rhs), loc);
		else
			tmp = addDebugLoc(context, context.getBuilder().CreateSDiv(lhs, rhs), loc);
		if (hasAssign(op)) {
			tmp = Type::cast(context, ansType, tmp, left->getType(context));
			addDebugLoc(
					context,
					left->store(context, tmp),
					loc);
		}
		return tmp; }
	case MOD_ASSIGN:
	case MOD: {
		llvm::Value *tmp;
		if (ansType->isUnsigned)
			tmp = addDebugLoc(context, context.getBuilder().CreateURem(lhs, rhs), loc);
		else
			tmp = addDebugLoc(context, context.getBuilder().CreateSRem(lhs, rhs), loc);
		if (hasAssign(op)) {
			tmp = Type::cast(context, ansType, tmp, left->getType(context));
			addDebugLoc(
					context,
					left->store(context, tmp),
					loc);
		}
		return tmp; }
	case LSH_ASSIGN:
	case LSH: {
		llvm::Value *tmp = addDebugLoc(context, context.getBuilder().CreateShl(lhs, rhs), loc);
		if (hasAssign(op)) {
			tmp = Type::cast(context, ansType, tmp, left->getType(context));
			addDebugLoc(
					context,
					left->store(context, tmp),
					loc);
		}
		return tmp; }
	case RSH_ASSIGN:
	case RSH: {
		llvm::Value *tmp;
		if (ansType->isUnsigned)
			tmp = addDebugLoc(context, context.getBuilder().CreateLShr(lhs, rhs), loc);
		else
			tmp = addDebugLoc(context, context.getBuilder().CreateAShr(lhs, rhs), loc);
		if (hasAssign(op)) {
			tmp = Type::cast(context, ansType, tmp, left->getType(context));
			addDebugLoc(
					context,
					left->store(context, tmp),
					loc);
		}
		return tmp; }
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
		// this has been handled before
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
	case LOG_AND_ASSIGN:
	case BIT_AND_ASSIGN:
	case LOG_AND:
	case BIT_AND: {
		llvm::Value *tmp = context.getBuilder().CreateAnd(lhs, rhs);
		if (hasAssign(op)) {
			tmp = Type::cast(context, ansType, tmp, left->getType(context));
			addDebugLoc(
					context,
					left->store(context, tmp),
					loc);
		}
		return tmp; }
	case LOG_OR_ASSIGN:
	case BIT_OR_ASSIGN:
	case LOG_OR:
	case BIT_OR: {
		llvm::Value *tmp = context.getBuilder().CreateOr(lhs, rhs);
		if (hasAssign(op)) {
			tmp = Type::cast(context, ansType, tmp, left->getType(context));
			addDebugLoc(
					context,
					left->store(context, tmp),
					loc);
		}
		return tmp; }
	case LOG_XOR_ASSIGN:
	case BIT_XOR_ASSIGN:
	case LOG_XOR:
	case BIT_XOR: {
		llvm::Value *tmp = context.getBuilder().CreateXor(lhs, rhs);
		if (hasAssign(op)) {
			tmp = Type::cast(context, ansType, tmp, left->getType(context));
			addDebugLoc(
					context,
					left->store(context, tmp),
					loc);
		}
		return tmp; }
	case PWR:
	case PWR_ASSIGN:
		break;
	}
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
	case MOD:
	case PWR:
	case LSH:
	case RSH:
	case BIT_AND:
	case BIT_OR:
	case BIT_XOR:
		return Type::higherType(left->getType(context), right->getType(context));
	case ADD_ASSIGN:
	case SUB_ASSIGN:
	case MUL_ASSIGN:
	case DIV_ASSIGN:
	case MOD_ASSIGN:
	case PWR_ASSIGN:
	case BIT_OR_ASSIGN:
	case BIT_AND_ASSIGN:
	case BIT_XOR_ASSIGN:
	case LOG_OR_ASSIGN:
	case LOG_AND_ASSIGN:
	case LOG_XOR_ASSIGN:
	case LSH_ASSIGN:
	case RSH_ASSIGN:
	case ASSIGN:
		return left->getType(context);
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
	}
}

bool Op2::isConstant() {
	return left->isConstant() && right->isConstant();
}

Expression::Constant Op2::loadConstant() {
	Type* higherType = Type::higherType(left->getTypeConstant(), right->getTypeConstant());
	Expression::Constant ans;
	switch (op) {
	case ADD_ASSIGN:
	case SUB_ASSIGN:
	case MUL_ASSIGN:
	case DIV_ASSIGN:
	case MOD_ASSIGN:
	case PWR_ASSIGN:
	case BIT_OR_ASSIGN:
	case BIT_AND_ASSIGN:
	case BIT_XOR_ASSIGN:
	case LOG_OR_ASSIGN:
	case LOG_AND_ASSIGN:
	case LOG_XOR_ASSIGN:
	case LSH_ASSIGN:
	case RSH_ASSIGN:
		throw InvalidType(std::string("can't apply operator ") + OpNames[op] + " to constant");
	default:
		break;
	}
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
			case MOD:
				if (left->getTypeConstant()->isUnsigned)
					ans._uint64 = left->loadConstant()._uint64;
				else
					ans._uint64 = left->loadConstant()._int64;
				if (right->getTypeConstant()->isUnsigned)
					ans._uint64 %= right->loadConstant()._uint64;
				else
					ans._uint64 %= right->loadConstant()._int64;
				return ans;
			case PWR:
				if (left->getTypeConstant()->isUnsigned)
					ans._uint64 = left->loadConstant()._uint64;
				else
					ans._uint64 = left->loadConstant()._int64;
				if (right->getTypeConstant()->isUnsigned)
					ans._uint64 = pow(ans._uint64, right->loadConstant()._uint64);
				else
					ans._uint64 = pow(ans._uint64, right->loadConstant()._int64);
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
			case MOD:
				if (left->getTypeConstant()->isUnsigned)
					ans._int64 = left->loadConstant()._uint64;
				else
					ans._int64 = left->loadConstant()._int64;
				if (right->getTypeConstant()->isUnsigned)
					ans._int64 %= right->loadConstant()._uint64;
				else
					ans._int64 %= right->loadConstant()._int64;
				return ans;
			case PWR:
				if (left->getTypeConstant()->isUnsigned)
					ans._int64 = left->loadConstant()._uint64;
				else
					ans._int64 = left->loadConstant()._int64;
				if (right->getTypeConstant()->isUnsigned)
					ans._int64 = pow(ans._int64, right->loadConstant()._uint64);
				else
					ans._int64 = pow(ans._int64, right->loadConstant()._int64);
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
		case LSH:
		case RSH:
		case MOD:
			throw InvalidType(std::string("can't apply operator ") + OpNames[op] + " to " + left->getTypeConstant()->getName() + " and " + right->getTypeConstant()->getName());
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
		throw NotImplemented("constant string calculate");
	throw NotImplemented(std::string("constant caculation ") + left->getTypeConstant()->getName() +
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
		throw NotImplemented(std::string("constant caculation ") + left->getTypeConstant()->getName() +
			OpNames[op] + right->getTypeConstant()->getName());
	}
}

bool Op2::hasAssign(OpType op) {
	switch (op) {
	case ADD:
	case SUB:
	case MUL:
	case DIV:
	case MOD:
	case PWR:
	case LT:
	case GT:
	case LEQ:
	case GEQ:
	case NEQ:
	case EQ:
	case LOG_AND:
	case LOG_OR:
	case LOG_XOR:
	case BIT_OR:
	case BIT_AND:
	case BIT_XOR:
	case LSH:
	case RSH:
		return false;
	case ADD_ASSIGN:
	case SUB_ASSIGN:
	case MUL_ASSIGN:
	case DIV_ASSIGN:
	case MOD_ASSIGN:
	case PWR_ASSIGN:
	case LOG_AND_ASSIGN:
	case LOG_OR_ASSIGN:
	case LOG_XOR_ASSIGN:
	case BIT_OR_ASSIGN:
	case BIT_AND_ASSIGN:
	case BIT_XOR_ASSIGN:
	case ASSIGN:
	case LSH_ASSIGN:
	case RSH_ASSIGN:
		return true;
	}
}
