#include <llvm/IR/IRBuilder.h>

#include "Op1.h"
#include "Context.h"
#include "exception.h"

const char* Op1::OpNames[] = {
	"++",
	"--",
	"~",
	"!"
};

Op1::Op1(ASTNode *operand, OpType op) :
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

void* Op1::gen(Context &context) {
	return NULL;
}
