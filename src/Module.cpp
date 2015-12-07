#include "Module.h"
#include "Namespace.h"
#include "Context.h"
#include "Class.h"

Module::Module(Namespace *ns, std::list<StructNode*> *definations) :
	ns(ns), list(*definations), parent(NULL) {
}

Module::~Module() {
	delete ns;
	for (std::list<StructNode*>::iterator it = list.begin(); it != list.end(); it++)
		delete *it;
	delete &list;
}

Json::Value Module::json() {
	Json::Value root;
	root["name"] = "module";
	root["ns"] = ns->json();
	root["definations"] = Json::Value(Json::arrayValue);
	int i = 0;
	for (std::list<StructNode*>::iterator it = list.begin(); it != list.end(); i++, it++)
		root["definations"][i] = (*it)->json();
	return root;
}

void Module::genStruct(Context &context) {
	context.currentModule = this;
	for (std::list<StructNode*>::iterator it = list.begin(); it != list.end(); it++) {
		(*it)->module = this;
		(*it)->genStruct(context);
	}
	context.currentModule = parent;
}

void Module::gen(Context &context) {
	context.currentModule = this;
	for (std::list<StructNode*>::iterator it = list.begin(); it != list.end(); it++)
		(*it)->gen(context);
	context.currentModule = parent;
}

const std::string Module::getFullName() {
	if (parent)
		return parent->getFullName() + "::" + ns->getFullName();
	else
		return ns->getFullName();
}

const std::string Module::getMangleName() {
	if (parent)
		return parent->getFullName() + ns->getMangleName();
	else
		return ns->getMangleName();
}
