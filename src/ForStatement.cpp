#include "ForStatement.h"
#include "Context.h"
#include "VariableDefination.h"
#include "Expression.h"
#include "util.h"
#include "DebugInfo.h"
#include "Type.h"
#include "exception.h"

ForStatement::ForStatement(VariableDefination *var, Expression *test, ASTNode *inc, ASTNode *body) :
		var(var), test(test), inc(inc), body(body) {
}

ForStatement::~ForStatement() {
	if (var)
		delete var;
	if (test)
		delete test;
	if (inc)
		delete inc;
	if (body)
		delete body;
}

Json::Value ForStatement::json() {
	Json::Value root;
	root["name"] = "for_statement";
	if (var)
		root["vars"] = var->json();
	if (test)
		root["test"] = test->json();
	if (inc)
		root["inc"] = inc->json();
	root["body"] = body->json();
	return root;
}

void ForStatement::gen(Context &context) {
	context.pushContext();
	if (var)
		var->gen(context);
	llvm::BasicBlock *oriBlock = context.currentBlock();
	llvm::BasicBlock *loopBlock = context.newBlock("for_" + itos(loc.begin.line) + "@loop");
	llvm::BasicBlock *bodyBlock = context.newBlock("for_" + itos(loc.begin.line) + "@body");
	llvm::BasicBlock *afterBlock = context.newBlock("for_" + itos(loc.begin.line) + "@after");

	context.setBlock(oriBlock);
	addDebugLoc(context, context.getBuilder().CreateBr(loopBlock), loc);

	context.setBlock(loopBlock);
	if (test) {
		llvm::Value *cond = test->load(context);
		if (!test->getType(context)->isBool())
			throw InvalidType("The test expression of while statement must be boolean");
		addDebugLoc(context, context.getBuilder().CreateCondBr(cond, bodyBlock, afterBlock), loc);
	} else
		addDebugLoc(context, context.getBuilder().CreateBr(bodyBlock), loc);

	context.setBlock(bodyBlock);
	body->gen(context);
	inc->gen(context);
	Location tmploc;
	tmploc.begin = loc.end;
	addDebugLoc(context, context.getBuilder().CreateBr(loopBlock), tmploc);

	context.setBlock(afterBlock);
	context.popContext();
}
