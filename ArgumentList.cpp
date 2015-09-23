#include "yyvaltypes.h"

ArgumentList::ArgumentList() {
}

ArgumentList::ArgumentList(ASTNode *type, ASTNode *identifier) {
	push_back(type, identifier);
}

void ArgumentList::push_back(ASTNode *type, ASTNode *identifier) {
	list.push_back(std::make_pair(type, identifier));
}

ArgumentList::~ArgumentList() {
	for (std::list<std::pair<ASTNode*,ASTNode*> >::iterator it = list.begin(); it != list.end(); it++) {
		if (it->first)
			delete it->first;
		if (it->second)
			delete it->second;
	}
}

Json::Value ArgumentList::json() {
	Json::Value root;
	root["name"] = "arg_list";
	root["list"] = Json::Value(Json::arrayValue);
	int i = 0;
	for (std::list<std::pair<ASTNode*,ASTNode*> >::iterator it = list.begin(); it != list.end(); it++, i++) {
		root["list"][i] = Json::Value();
		root["list"][i]["type"] = it->first->json();
		root["list"][i]["identifier"] = it->second->json();
	}
	return root;
}
