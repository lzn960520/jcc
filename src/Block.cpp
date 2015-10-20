#include "Block.h"
#include "Context.h"

Block::Block(ASTNode *body):
	body(body) {}

Block::~Block() {
	if (body)
		delete body;
}

Json::Value Block::json() {
	Json::Value root;
	root["name"] = "block";
	root["body"] = body->json();
	return root;
}

void Block::gen(Context &context) {
	context.pushContext();
	body->gen(context);
	context.popContext();
}
