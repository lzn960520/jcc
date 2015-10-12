#include <llvm/IR/IRBuilder.h>

#include "Op2.h"
#include "Context.h"
#include "exception.h"

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
	"="
};

Op2::Op2(ASTNode *left, OpType op, ASTNode *right) :
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

void* Op2::gen(Context &context) {
	llvm::Value *lhs = (llvm::Value *) left->gen(context);
	llvm::Value *rhs = (llvm::Value *) right->gen(context);
	switch (op) {
	case ADD:
		return context.getBuilder().CreateAdd(lhs, rhs);
	case SUB:
		return context.getBuilder().CreateSub(lhs, rhs);
	case MUL:
		return context.getBuilder().CreateMul(lhs, rhs);
	case DIV:
		return context.getBuilder().CreateSDiv(lhs, rhs);
	default:
		throw NotImplemented(std::string("operator ") + OpNames[op] + " not implemented");
	}
	return NULL;
}
