#include "IfStatement.h"
#include "Context.h"
#include "util.h"

IfStatement::IfStatement(ASTNode *test, ASTNode *then_st) :
	test(test), then_st(then_st), else_st(NULL) {
}

IfStatement::IfStatement(ASTNode *test, ASTNode *then_st, ASTNode *else_st) :
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

void* IfStatement::gen(Context &context) {
	llvm::IRBuilderBase::InsertPoint condIP = context.getBuilder().saveAndClearIP(), afterThenIP, afterElseIP;
	llvm::BasicBlock *true_b, *false_b;
	if (then_st) {
		true_b = context.newBlock("if_" + itos(then_st->loc.first_line) + "@true");
		then_st->gen(context);
		afterThenIP = context.getBuilder().saveAndClearIP();
	}
	if (else_st) {
		false_b = context.newBlock("if_" + itos(else_st->loc.first_line) + "@false");
		else_st->gen(context);
		afterElseIP = context.getBuilder().saveAndClearIP();
	}
	llvm::BasicBlock *after = context.newBlock("if_" + itos(then_st->loc.first_line) + "@after");
	llvm::IRBuilderBase::InsertPoint afterIP = context.getBuilder().saveIP();
	if (then_st) {
		context.getBuilder().restoreIP(afterThenIP);
		context.getBuilder().CreateBr(after);
	}
	if (else_st) {
		context.getBuilder().restoreIP(afterElseIP);
		context.getBuilder().CreateBr(after);
	}
	if (!else_st)
		false_b = after;
	context.getBuilder().restoreIP(condIP);
	llvm::Value *cond = (llvm::Value *) test->gen(context);
	if (!cond->getType()->isIntegerTy(1) && cond->getType()->isIntegerTy())
		cond = context.getBuilder().CreateCast(llvm::Instruction::Trunc, cond, context.getBuilder().getInt1Ty());
	context.getBuilder().CreateCondBr(cond, true_b, false_b);
	context.getBuilder().restoreIP(afterIP);
	return NULL;
}
