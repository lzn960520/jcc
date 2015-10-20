#include <llvm/IR/IRBuilder.h>

#include "Op2.h"
#include "Context.h"
#include "exception.h"
#include "Type.h"
#include "Expression.h"

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
	"^^"
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
	llvm::Value *lhs = left->load(context);
	llvm::Value *rhs = right->load(context);
	Type *ansType = Type::higherType(left->getType(context), right->getType(context));
	switch (op) {
	case ADD:
		return context.getBuilder().CreateAdd(lhs, rhs);
	case SUB:
		return context.getBuilder().CreateSub(lhs, rhs);
	case MUL:
		return context.getBuilder().CreateMul(lhs, rhs);
	case DIV:
		return context.getBuilder().CreateSDiv(lhs, rhs);
	case LT:
		if (left->getType(context)->isFloat() || right->getType(context)->isFloat()) {
			if (!left->getType(context)->isFloat())
				lhs = context.getBuilder().CreateFPCast(lhs, context.getBuilder().getFloatTy());
			if (!right->getType(context)->isFloat())
				rhs = context.getBuilder().CreateFPCast(rhs, context.getBuilder().getFloatTy());
			return context.getBuilder().CreateFCmpOLT(lhs, rhs);
		} else if (left->getType(context)->isUnsigned || right->getType(context)->isUnsigned) {
			lhs = context.getBuilder().CreateIntCast(lhs, context.getBuilder().getInt32Ty(), false);
			rhs = context.getBuilder().CreateIntCast(rhs, context.getBuilder().getInt32Ty(), false);
			return context.getBuilder().CreateICmpULT(lhs, rhs);
		} else if (ansType->isString())
			throw NotImplemented("string operation");
		else {
			lhs = context.getBuilder().CreateIntCast(lhs, context.getBuilder().getInt32Ty(), true);
			rhs = context.getBuilder().CreateIntCast(rhs, context.getBuilder().getInt32Ty(), true);
			return context.getBuilder().CreateICmpSLT(lhs, rhs);
		}
	case LEQ:
		if (left->getType(context)->isFloat() || right->getType(context)->isFloat()) {
			if (!left->getType(context)->isFloat())
				lhs = context.getBuilder().CreateFPCast(lhs, context.getBuilder().getFloatTy());
			if (!right->getType(context)->isFloat())
				rhs = context.getBuilder().CreateFPCast(rhs, context.getBuilder().getFloatTy());
			return context.getBuilder().CreateFCmpOLT(lhs, rhs);
		} else if (left->getType(context)->isUnsigned || right->getType(context)->isUnsigned) {
			lhs = context.getBuilder().CreateIntCast(lhs, context.getBuilder().getInt32Ty(), false);
			rhs = context.getBuilder().CreateIntCast(rhs, context.getBuilder().getInt32Ty(), false);
			return context.getBuilder().CreateICmpULE(lhs, rhs);
		} else if (ansType->isString())
			throw NotImplemented("string operation");
		else {
			lhs = context.getBuilder().CreateIntCast(lhs, context.getBuilder().getInt32Ty(), true);
			rhs = context.getBuilder().CreateIntCast(rhs, context.getBuilder().getInt32Ty(), true);
			return context.getBuilder().CreateICmpSLE(lhs, rhs);
		}
	case GT:
		if (left->getType(context)->isFloat() || right->getType(context)->isFloat()) {
			if (!left->getType(context)->isFloat())
				lhs = context.getBuilder().CreateFPCast(lhs, context.getBuilder().getFloatTy());
			if (!right->getType(context)->isFloat())
				rhs = context.getBuilder().CreateFPCast(rhs, context.getBuilder().getFloatTy());
			return context.getBuilder().CreateFCmpOLT(lhs, rhs);
		} else if (left->getType(context)->isUnsigned || right->getType(context)->isUnsigned) {
			lhs = context.getBuilder().CreateIntCast(lhs, context.getBuilder().getInt32Ty(), false);
			rhs = context.getBuilder().CreateIntCast(rhs, context.getBuilder().getInt32Ty(), false);
			return context.getBuilder().CreateICmpUGT(lhs, rhs);
		} else if (ansType->isString())
			throw NotImplemented("string operation");
		else {
			lhs = context.getBuilder().CreateIntCast(lhs, context.getBuilder().getInt32Ty(), true);
			rhs = context.getBuilder().CreateIntCast(rhs, context.getBuilder().getInt32Ty(), true);
			return context.getBuilder().CreateICmpSGT(lhs, rhs);
		}
	case GEQ:
		if (left->getType(context)->isFloat() || right->getType(context)->isFloat()) {
			if (!left->getType(context)->isFloat())
				lhs = context.getBuilder().CreateFPCast(lhs, context.getBuilder().getFloatTy());
			if (!right->getType(context)->isFloat())
				rhs = context.getBuilder().CreateFPCast(rhs, context.getBuilder().getFloatTy());
			return context.getBuilder().CreateFCmpOLT(lhs, rhs);
		} else if (left->getType(context)->isUnsigned || right->getType(context)->isUnsigned) {
			lhs = context.getBuilder().CreateIntCast(lhs, context.getBuilder().getInt32Ty(), false);
			rhs = context.getBuilder().CreateIntCast(rhs, context.getBuilder().getInt32Ty(), false);
			return context.getBuilder().CreateICmpUGE(lhs, rhs);
		} else if (ansType->isString())
			throw NotImplemented("string operation");
		else {
			lhs = context.getBuilder().CreateIntCast(lhs, context.getBuilder().getInt32Ty(), true);
			rhs = context.getBuilder().CreateIntCast(rhs, context.getBuilder().getInt32Ty(), true);
			return context.getBuilder().CreateICmpSGE(lhs, rhs);
		}
	case ASSIGN:
		left->store(context, right->load(context));
		break;
	case EQ:
		if (left->getType(context)->isFloat() || right->getType(context)->isFloat()) {
			if (!left->getType(context)->isFloat())
				lhs = context.getBuilder().CreateFPCast(lhs, context.getBuilder().getFloatTy());
			if (!right->getType(context)->isFloat())
				rhs = context.getBuilder().CreateFPCast(rhs, context.getBuilder().getFloatTy());
			return context.getBuilder().CreateFCmpOEQ(lhs, rhs);
		} else if (left->getType(context)->isUnsigned || right->getType(context)->isUnsigned) {
			lhs = context.getBuilder().CreateIntCast(lhs, context.getBuilder().getInt32Ty(), false);
			rhs = context.getBuilder().CreateIntCast(rhs, context.getBuilder().getInt32Ty(), false);
			return context.getBuilder().CreateICmpEQ(lhs, rhs);
		} else if (ansType->isString())
			throw NotImplemented("string operation");
		else {
			lhs = context.getBuilder().CreateIntCast(lhs, context.getBuilder().getInt32Ty(), true);
			rhs = context.getBuilder().CreateIntCast(rhs, context.getBuilder().getInt32Ty(), true);
			return context.getBuilder().CreateICmpEQ(lhs, rhs);
		}
	case NEQ:
		if (left->getType(context)->isFloat() || right->getType(context)->isFloat()) {
			if (!left->getType(context)->isFloat())
				lhs = context.getBuilder().CreateFPCast(lhs, context.getBuilder().getFloatTy());
			if (!right->getType(context)->isFloat())
				rhs = context.getBuilder().CreateFPCast(rhs, context.getBuilder().getFloatTy());
			return context.getBuilder().CreateFCmpONE(lhs, rhs);
		} else if (left->getType(context)->isUnsigned || right->getType(context)->isUnsigned) {
			lhs = context.getBuilder().CreateIntCast(lhs, context.getBuilder().getInt32Ty(), false);
			rhs = context.getBuilder().CreateIntCast(rhs, context.getBuilder().getInt32Ty(), false);
			return context.getBuilder().CreateICmpNE(lhs, rhs);
		} else if (ansType->isString())
			throw NotImplemented("string operation");
		else {
			lhs = context.getBuilder().CreateIntCast(lhs, context.getBuilder().getInt32Ty(), true);
			rhs = context.getBuilder().CreateIntCast(rhs, context.getBuilder().getInt32Ty(), true);
			return context.getBuilder().CreateICmpNE(lhs, rhs);
		}
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
	return NULL;
}

void Op2::store(Context &context, llvm::Value *value) {
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
