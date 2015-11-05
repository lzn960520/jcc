#include "Namespace.h"
#include "Identifier.h"
#include "util.h"

Namespace::Namespace(Identifier *identifier) {
	push_back(identifier);
}

void Namespace::push_back(Identifier *identifier) {
	list.push_back(identifier);
}

Namespace::~Namespace() {
	for (std::list<Identifier*>::iterator it = list.begin(); it != list.end(); it++)
		delete *it;
}

Json::Value Namespace::json() {
	Json::Value root;
	root["name"] = "namespace";
	root["ns"] = getFullName();
	return root;
}

const std::string Namespace::getMangleName() {
	std::string tmp;
	for (std::list<Identifier*>::iterator it = list.begin(); it != list.end(); it++)
		tmp = tmp + "N" + itos((*it)->getName().length()) + (*it)->getName();
	return tmp;
}

const std::string Namespace::getFullName() {
	std::list<Identifier*>::iterator it = list.begin();
	std::string tmp = (*it++)->getName();
	for (; it != list.end(); it++)
		tmp = tmp + "::" + (*it)->getName();
	return tmp;
}
