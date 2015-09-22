#include "yyvaltypes.h"

Identifier::Identifier(const char *name) :
	text(name) {
}

Json::Value Identifier::json() {
	Json::Value root;
	root["name"] = "identifier";
	root["text"] = Json::Value(text);
	return root;
}

Identifier::~Identifier() {
}
