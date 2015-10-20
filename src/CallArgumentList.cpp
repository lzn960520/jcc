#include "CallArgumentList.h"
#include "Expression.h"

CallArgumentList::CallArgumentList() {
}

CallArgumentList::CallArgumentList(Expression *expression) {
	push_back(expression);
}

void CallArgumentList::push_back(Expression *expression) {
	if (expression)
		list.push_back(expression);
}

CallArgumentList::~CallArgumentList() {
	for (std::list<Expression*>::iterator it = list.begin(); it != list.end(); it++) {
		if (*it)
			delete *it;
	}
}

Json::Value CallArgumentList::json() {
	Json::Value root;
	root["name"] = "call_arg_list";
	root["list"] = Json::Value(Json::arrayValue);
	int i = 0;
	for (std::list<Expression*>::iterator it = list.begin(); it != list.end(); it++, i++)
		root["list"][i] = (*it)->json();
	return root;
}
