#include "ArrayDefinator.h"
#include "Expression.h"

ArrayDefinator::ArrayDefinator(Expression *lower, Expression *upper) {
	push_back(lower, upper);
}

void ArrayDefinator::push_back(Expression *lower, Expression *upper) {
	list.push_back(std::make_pair(lower, upper));
}

ArrayDefinator::~ArrayDefinator() {
	for (std::vector<std::pair<Expression *, Expression *> >::iterator it = list.begin(); it != list.end(); it++) {
		if (it->first)
			delete it->first;
		if (it->second)
			delete it->second;
	}
}

Json::Value ArrayDefinator::json() {
	return Json::Value();
}
