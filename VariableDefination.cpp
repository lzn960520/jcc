#include "yyvaltypes.h"

VariableDefination::VariableDefination(ASTNode *type) :
	type(type) {
}

void VariableDefination::push_back(ASTNode *identifier) {
	list.push_back(std::make_pair<ASTNode*,ASTNode*>(identifier, NULL));
}

void VariableDefination::push_back(ASTNode *identifier, ASTNode *expression) {
	list.push_back(std::make_pair(identifier, expression));
}

VariableDefination::~VariableDefination() {
	for (std::list<std::pair<ASTNode*,ASTNode*> >::iterator it = list.begin(); it != list.end(); it++) {
		if (it->first)
			delete it->first;
		if (it->second)
			delete it->second;
	}
	if (type)
		delete type;
}

Json::Value VariableDefination::json() {
	Json::Value root;
	root["name"] = "variable_defination";
	root["type"] = type->json();
	root["list"] = Json::Value(Json::arrayValue);
	int i = 0;
	for (std::list<std::pair<ASTNode*,ASTNode*> >::iterator it = list.begin(); it != list.end(); it++, i++) {
		root["list"][i] = Json::Value();
		root["list"][i]["identifier"] = it->first->json();
		if (it->second)
			root["list"][i]["init_value"] = it->second->json();
	}
	return root;
}
