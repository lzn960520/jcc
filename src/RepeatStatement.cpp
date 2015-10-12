#include "RepeatStatement.h"
#include "Context.h"
#include "util.h"
#include "exception.h"

RepeatStatement::RepeatStatement(ASTNode *body, ASTNode *until) :
	body(body), until(until) {
}

RepeatStatement::~RepeatStatement() {
	if (body)
		delete body;
	if (until)
		delete until;
}

Json::Value RepeatStatement::json() {
	Json::Value root;
	root["name"] = "repeat_statement";
	root["body"] = body->json();
	root["until"] = until->json();
	return root;
}

void* RepeatStatement::gen(Context &context) {
	llvm::BasicBlock *oriBlock = context.currentBlock();
	llvm::BasicBlock *loopBlock = context.newBlock("repeat_" + itos(loc.first_line) + "@loop");
	llvm::BasicBlock *afterBlock = context.newBlock("repeat_" + itos(loc.first_line) + "@after");

	context.setBlock(oriBlock);
	context.getBuilder().CreateBr(loopBlock);

	context.setBlock(loopBlock);
	body->gen(context);
	llvm::Value *cond = (llvm::Value *) until->gen(context);
	if (!cond->getType()->isIntegerTy(1))
		if (cond->getType()->isIntegerTy())
			cond = context.getBuilder().CreateCast(llvm::Instruction::Trunc, cond, context.getBuilder().getInt1Ty());
		else
			throw CompileException("The expression can't be converted to boolean");
	context.getBuilder().CreateCondBr(cond, afterBlock, loopBlock);

	context.setBlock(afterBlock);
	return NULL;
}
