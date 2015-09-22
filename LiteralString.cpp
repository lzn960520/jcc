#include "yyvaltypes.h"

LiteralString::LiteralString(const char *text) :
	text(text) {
}

Json::Value LiteralString::json() {
	Json::Value root;
	root["name"] = "literal_string";
	root["text"] = Json::Value(text);
	return root;
}

LiteralString::~LiteralString() {
}
