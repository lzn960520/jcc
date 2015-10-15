#include "Module.h"
#include "Namespace.h"
#include "Context.h"
#include "Class.h"

Module::Module(Namespace *ns, std::list<ASTNode*> *definations) :
	ns(ns), list(*definations) {
}

Module::~Module() {
	delete ns;
	for (std::list<ASTNode*>::iterator it = list.begin(); it != list.end(); it++)
		delete *it;
	delete &list;
}

Json::Value Module::json() {
	Json::Value root;
	root["name"] = "module";
	root["ns"] = ns->json();
	root["definations"] = Json::Value(Json::arrayValue);
	int i = 0;
	for (std::list<ASTNode*>::iterator it = list.begin(); it != list.end(); i++, it++)
		root["definations"][i] = (*it)->json();
	return root;
}

void Module::genStruct(Context &context) {
	for (std::list<ASTNode*>::iterator it = list.begin(); it != list.end(); it++)
		if (typeid(**it) == typeid(Class)) {
			((Class *) *it)->setNS(ns->getPrefix());
			((Class *) *it)->genStruct(context);
		}
}

void Module::gen(Context &context) {
	for (std::list<ASTNode*>::iterator it = list.begin(); it != list.end(); it++)
		(*it)->gen(context);
}
