#include "CompileFile.h"
#include "Module.h"
#include "util.h"
#include "JsymFile.h"

Json::Value CompileFile::json() {
	Json::Value root;
	root["name"] = "file";
	root["usings"] = Json::Value(Json::arrayValue);
	int i = 0;
	for (std::list<std::string>::iterator it = usings.begin(); it != usings.end(); it++, i++)
		root["usings"][i] = *it;
	i = 0;
	root["modules"] = Json::Value(Json::arrayValue);
	for (std::list<Module*>::iterator it = modules.begin(); it != modules.end(); it++, i++)
		root["modules"][i] = (*it)->json();
	return root;
}

void CompileFile::gen(Context &context) {
	for (std::list<Module*>::iterator it = modules.begin(); it != modules.end(); it++)
		(*it)->gen(context);
}

void CompileFile::genStruct(Context &context) {
	for (std::list<std::string>::iterator it = usings.begin(); it != usings.end(); it++) {
		JsymFile jsym(resolveRelativePath(path, *it), true);
		jsym >> context;
	}
	for (std::list<Module*>::iterator it = modules.begin(); it != modules.end(); it++)
		(*it)->genStruct(context);
}
