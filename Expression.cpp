#include "jascal.tab.hpp"
#include "yyvaltypes.h"
#include <llvm/IR/IRBuilder.h>

const char* Expression::OpNames(OpType op) {
	switch (op) {
	case ADD:
		return "ADD";
	case SUB:
		return "SUB";
	case MUL:
		return "MUL";
	case DIV:
		return "DIV";
	case PWR:
		return "PWR";
	case MOD:
		return "MOD";
	}
}

const char *Expression::TypeNames[] = {
	"op2",
	"op1",
	"literal",
	"enclosed",
	"identifier",
	"function call",
	"new",
	"delete"
};

Expression::Expression(ASTNode *single, Type type) :
	left(single), type(type) {
}

Expression::Expression(ASTNode *left, OpType op, ASTNode *right) :
	left(left), right(right), op(op), type(OP2) {
}

Json::Value Expression::json() {
	Json::Value root;
	root["name"] = "expression";
	root["type"] = TypeNames[type];
	switch (type) {
	case OP2:
		root["left"] = left->json();
		root["right"] = right->json();
		root["op"] = OpNames(op);
		break;
	case LITERAL:
		return left->json();
		break;
	case IDENTIFIER:
		return left->json();
		break;
	}
	return root;
}

Expression::~Expression() {
	if (left)
		delete left;
	if (right)
		delete right;
}

void* Expression::gen(Context &context) {
	switch (type) {
	case OP2:
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
		}
		break;
	}
	return NULL;
}
