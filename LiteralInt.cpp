#include "yyvaltypes.h"

LiteralInt::LiteralInt(int val) :
	val(val) {
}

Json::Value LiteralInt::json() {
	Json::Value root;
	root["name"] = "literal_int";
	root["val"] = Json::Value(val);
	return root;
}

LiteralInt::~LiteralInt() {
}
