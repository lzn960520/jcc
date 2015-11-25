#include "Interface.h"
#include "Identifier.h"
#include "MemberNode.h"

Interface::Interface(Identifier *identifier, std::list<MemberNode*> *list) :
		identifier(identifier), list(*list) {
}

Interface::~Interface() {
	delete identifier;
	delete &list;
}

Json::Value Interface::json() {
	Json::Value root;
	root["name"] = "interface";
	root["identifier"] = identifier->json();
	root["definations"] = Json::Value(Json::arrayValue);
	int i = 0;
	for (std::list<MemberNode*>::iterator it = list.begin(); it != list.end(); it++, i++)
		root["definations"][i] = (*it)->json();
	return root;
}

void Interface::gen(Context &context) {
}

void Interface::genStruct(Context &context) {
}

const std::string Interface::getName() {
}
