#include "yyvaltypes.h"

Function::Function(Visibility visibility, ASTNode *return_type, ASTNode *identifier, ASTNode *arg_list, ASTNode *body) :
	visibility(visibility), return_type(return_type), identifier(identifier), arg_list(arg_list), body(body) {
}

Function::~Function() {
	if (return_type)
		delete return_type;
	if (identifier)
		delete identifier;
	if (arg_list)
		delete arg_list;
	if (body)
		delete body;
}

Json::Value Function::json() {
	Json::Value root;
	root["name"] = "function";
	root["visibility"] = VisibilityNames[visibility];
	root["identifier"] = identifier->json();
	root["arg_list"] = arg_list->json();
	root["body"] = body->json();
	return root;
}
