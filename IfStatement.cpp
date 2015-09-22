#include "yyvaltypes.h"

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
