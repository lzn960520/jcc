#include "Namespace.h"
#include "Identifier.h"

Namespace::Namespace(Identifier *identifier) {
	push_back(identifier);
}

void Namespace::push_back(Identifier *identifier) {
	list.push_back(identifier);
	prefix += identifier->text + "$";
}

Namespace::~Namespace() {
	for (std::list<Identifier*>::iterator it = list.begin(); it != list.end(); it++)
		delete *it;
}

Json::Value Namespace::json() {
	Json::Value root;
	root["name"] = "namespace";
	std::list<Identifier*>::iterator it = list.begin();
	std::string tmp;
	tmp = (*it++)->getName();
	for (; it != list.end(); it++)
		tmp += "::" + (*it)->getName();
	root["ns"] = tmp;
	return root;
}
