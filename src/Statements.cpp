#include "Statements.h"

Statements::Statements() {
}

Statements::Statements(ASTNode *statement) {
	push_back(statement);
}

void Statements::push_back(ASTNode *statement) {
	if (statement)
		statements.push_back(statement);
}

Statements::~Statements() {
	for (std::list<ASTNode*>::iterator it = statements.begin(); it != statements.end(); it++)
		delete *it;
}

Json::Value Statements::json() {
	int i = 0;
	Json::Value root;
	root["name"] = "statements";
	root["statements"] = Json::Value(Json::arrayValue);
	for (std::list<ASTNode*>::iterator it = statements.begin(); it != statements.end(); it++, i++)
		root["statements"][i] = (*it)->json();
	return root;
}

void* Statements::gen(Context &context) {
	for (std::list<ASTNode*>::iterator it = statements.begin(); it != statements.end(); it++)
		(*it)->gen(context);
	return NULL;
}
