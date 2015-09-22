#include "yyvaltypes.h"

CompileUnit::CompileUnit(ASTNode *body) :
	body(body) {
}

Json::Value CompileUnit::json() {
	Json::Value root;
	root["body"] = body->json();
	root["name"] = "compile_unit";
	return root;
}

CompileUnit::~CompileUnit() {
	if (body)
		delete body;
}
