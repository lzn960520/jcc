#include "ArrayAccessor.h"
#include "Expression.h"

ArrayAccessor::ArrayAccessor(Expression *expression) {
	push_back(expression);
}

ArrayAccessor* ArrayAccessor::clone() const {
	ArrayAccessor *ans = new ArrayAccessor(list[0]->clone());
	for (size_t i = 1, len = list.size(); i != len; i++)
		ans->push_back(list[i]->clone());
	return ans;
}

void ArrayAccessor::push_back(Expression *expression) {
	list.push_back(expression);
}

ArrayAccessor::~ArrayAccessor() {
	for (std::vector<Expression*>::iterator it = list.begin(); it != list.end(); it++) {
		if (*it)
			delete *it;
	}
}

Json::Value ArrayAccessor::json() {
	Json::Value root;
	root["name"] = "array_accessor";
	root["list"] = Json::Value(Json::arrayValue);
	int i = 0;
	for (std::vector<Expression*>::iterator it = list.begin(); it != list.end(); it++, i++)
		root["list"][i] = (*it)->json();
	return root;
}
