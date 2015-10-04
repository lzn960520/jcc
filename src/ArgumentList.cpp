#include "ArgumentList.h"
#include "Type.h"
#include "Identifier.h"

ArgumentList::ArgumentList() {
}

ArgumentList::ArgumentList(Type *type, Identifier *identifier) {
	push_back(type, identifier);
}

void ArgumentList::push_back(Type *type, Identifier *identifier) {
	list.push_back(std::make_pair(type, identifier));
}

ArgumentList::~ArgumentList() {
	for (ListType::iterator it = list.begin(); it != list.end(); it++) {
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
	for (ListType::iterator it = list.begin(); it != list.end(); it++, i++) {
		root["list"][i] = Json::Value();
		root["list"][i]["type"] = it->first->json();
		root["list"][i]["identifier"] = it->second->json();
	}
	return root;
}
