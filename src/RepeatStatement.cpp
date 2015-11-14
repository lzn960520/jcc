#include "RepeatStatement.h"
#include "Context.h"
#include "util.h"
#include "exception.h"
#include "Expression.h"
#include "Type.h"

RepeatStatement::RepeatStatement(ASTNode *body, Expression *until) :
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

void RepeatStatement::gen(Context &context) {
	if (!until->getType(context)->isBool())
		throw InvalidType("test expression of repeat must be bool");
	llvm::BasicBlock *oriBlock = context.currentBlock();
	llvm::BasicBlock *loopBlock = context.newBlock("repeat_" + itos(loc.begin.line) + "@loop");
	llvm::BasicBlock *afterBlock = context.newBlock("repeat_" + itos(loc.begin.line) + "@after");

	context.setBlock(oriBlock);
	context.getBuilder().CreateBr(loopBlock);

	context.setBlock(loopBlock);
	body->gen(context);
	llvm::Value *cond = until->load(context);
	context.getBuilder().CreateCondBr(cond, afterBlock, loopBlock);

	context.setBlock(afterBlock);
}
