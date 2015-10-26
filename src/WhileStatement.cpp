#include "WhileStatement.h"
#include "Context.h"
#include "util.h"
#include "exception.h"
#include "Expression.h"
#include "Type.h"
#include "DebugInfo.h"

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

void WhileStatement::gen(Context &context) {
	llvm::BasicBlock *oriBlock = context.currentBlock();
	llvm::BasicBlock *loopBlock = context.newBlock("while_" + itos(loc.first_line) + "@loop");
	llvm::BasicBlock *bodyBlock = context.newBlock("while_" + itos(loc.first_line) + "@body");
	llvm::BasicBlock *afterBlock = context.newBlock("while_" + itos(loc.first_line) + "@after");

	context.setBlock(oriBlock);
	addDebugLoc(context, context.getBuilder().CreateBr(loopBlock), loc);

	context.setBlock(loopBlock);
	llvm::Value *cond = test->load(context);
	if (!test->getType(context)->isBool())
		throw InvalidType("The test expression of while statement must be boolean");
	addDebugLoc(context, context.getBuilder().CreateCondBr(cond, bodyBlock, afterBlock), loc);

	context.setBlock(bodyBlock);
	body->gen(context);
	YYLTYPE tmploc;
	tmploc.first_line = loc.first_line;
	tmploc.first_column = loc.first_column;
	addDebugLoc(context, context.getBuilder().CreateBr(loopBlock), tmploc);

	context.setBlock(afterBlock);
}
