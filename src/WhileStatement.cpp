#include "WhileStatement.h"
#include "Context.h"
#include "util.h"
#include "exception.h"
#include "Expression.h"

WhileStatement::WhileStatement(Expression *test, ASTNode *body) :
	test(test), body(body) {
}

WhileStatement::~WhileStatement() {
	if (test)
		delete test;
	if (body)
		delete body;
}

Json::Value WhileStatement::json() {
	Json::Value root;
	root["name"] = "while_statement";
	root["test"] = test->json();
	root["body"] = body->json();
	return root;
}

void* WhileStatement::gen(Context &context) {
	llvm::BasicBlock *oriBlock = context.currentBlock();
	llvm::BasicBlock *loopBlock = context.newBlock("while_" + itos(loc.first_line) + "@loop");
	llvm::BasicBlock *bodyBlock = context.newBlock("while_" + itos(loc.first_line) + "@body");
	llvm::BasicBlock *afterBlock = context.newBlock("while_" + itos(loc.first_line) + "@after");

	context.setBlock(oriBlock);
	context.getBuilder().CreateBr(loopBlock);

	context.setBlock(loopBlock);
	llvm::Value *cond = test->load(context);
	if (!cond->getType()->isIntegerTy(1))
		if (cond->getType()->isIntegerTy())
			cond = context.getBuilder().CreateCast(llvm::Instruction::Trunc, cond, context.getBuilder().getInt1Ty());
		else
			throw CompileException("The expression can't be converted to boolean");
	context.getBuilder().CreateCondBr(cond, bodyBlock, afterBlock);

	context.setBlock(bodyBlock);
	body->gen(context);
	context.getBuilder().CreateBr(loopBlock);

	context.setBlock(afterBlock);
	return NULL;
}
