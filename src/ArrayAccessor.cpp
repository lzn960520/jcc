#include "ArrayAccessor.h"

ArrayAccessor::ArrayAccessor(ASTNode *expression) {
	push_back(expression);
}

void ArrayAccessor::push_back(ASTNode *expression) {
	if (expression)
		list.push_back(expression);
}

ArrayAccessor::~ArrayAccessor() {
	for (std::list<ASTNode*>::iterator it = list.begin(); it != list.end(); it++) {
		if (*it)
			delete *it;
	}
}

Json::Value ArrayAccessor::json() {
	Json::Value root;
	root["name"] = "array_accessor";
	root["list"] = Json::Value(Json::arrayValue);
	int i = 0;
	for (std::list<ASTNode*>::iterator it = list.begin(); it != list.end(); it++, i++)
		root["list"][i] = (*it)->json();
	return root;
}
