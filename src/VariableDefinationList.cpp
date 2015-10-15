#include "VariableDefinationList.h"
#include "Identifier.h"
#include "Expression.h"

VariableDefinationList::VariableDefinationList(Identifier *identifier) {
	push_back(identifier);
}

VariableDefinationList::VariableDefinationList(Identifier *identifier, Expression *init_value) {
	push_back(identifier, init_value);
}

VariableDefinationList::~VariableDefinationList() {
	for (std::list<std::pair<Identifier*, Expression*> >::iterator it = list.begin(); it != list.end(); it++) {
		if (it->first)
			delete it->first;
		if (it->second)
			delete it->second;
	}
}

void VariableDefinationList::push_back(Identifier *identifier) {
	list.push_back(std::pair<Identifier*, Expression*>(identifier, NULL));
}

void VariableDefinationList::push_back(Identifier *identifier, Expression *init_value) {
	list.push_back(std::pair<Identifier*, Expression*>(identifier, init_value));
}

Json::Value VariableDefinationList::json() {
	Json::Value root(Json::arrayValue);
	int i = 0;
	for (std::list<std::pair<Identifier*, Expression*> >::iterator it = list.begin(); it != list.end(); it++, i++) {
		root[i] = Json::Value();
		if (it->first)
			root[i]["identifier"] = it->first->json();
		if (it->second)
			root[i]["init_value"] = it->second->json();
	}
	return root;
}
