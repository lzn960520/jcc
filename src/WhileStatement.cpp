#include "WhileStatement.h"

WhileStatement::WhileStatement(ASTNode *test, ASTNode *body) :
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
