#include "IfStatement.h"
#include "Context.h"
#include "util.h"
#include "Expression.h"
#include "exception.h"
#include "Type.h"
#include "DebugInfo.h"

IfStatement::IfStatement(Expression *test, ASTNode *then_st) :
	test(test), then_st(then_st), else_st(NULL) {
}

IfStatement::IfStatement(Expression *test, ASTNode *then_st, ASTNode *else_st) :
	test(test), then_st(then_st), else_st(else_st) {
}

IfStatement::~IfStatement() {
	if (test)
		delete test;
	if (then_st)
		delete then_st;
	if (else_st)
		delete else_st;
}

Json::Value IfStatement::json() {
	Json::Value root;
	root["name"] = "if_statement";
	root["test"] = test->json();
	root["then"] = then_st->json();
	if (else_st)
		root["else"] = else_st->json();
	return root;
}

void IfStatement::gen(Context &context) {
	if (!test->getType(context)->isBool())
		throw InvalidType("test expression of if must be bool");
	llvm::BasicBlock *ori_block = context.currentBlock(), *true_b, *false_b;
	if (then_st) {
		true_b = context.newBlock("if_" + itos(then_st->loc.first_line) + "@true");
		then_st->gen(context);
	}
	if (else_st) {
		false_b = context.newBlock("if_" + itos(else_st->loc.first_line) + "@false");
		else_st->gen(context);
	}
	llvm::BasicBlock *after = context.newBlock("if_" + itos(then_st->loc.first_line) + "@after");
	YYLTYPE tmploc;
	tmploc.first_line = loc.last_line;
	tmploc.first_column = loc.last_column;
	if (then_st) {
		context.setBlock(true_b);
		addDebugLoc(context, context.getBuilder().CreateBr(after), tmploc);
	}
	if (else_st) {
		context.setBlock(false_b);
		addDebugLoc(context, context.getBuilder().CreateBr(after), tmploc);
	}
	if (!else_st)
		false_b = after;
	context.setBlock(ori_block);
	llvm::Value *cond = test->load(context);
	addDebugLoc(context, context.getBuilder().CreateCondBr(cond, true_b, false_b), loc);
	context.setBlock(after);
}
