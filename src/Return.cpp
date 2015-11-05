#include "Return.h"
#include "Context.h"
#include "Expression.h"
#include "DebugInfo.h"

Return::Return() :
	expression(NULL) {
}

Return::Return(Expression *expression) :
	expression(expression) {
}

Return::~Return() {
	if (expression)
		delete expression;
}

Json::Value Return::json() {
	Json::Value root;
	root["name"] = "return";
	if (expression)
		root["value"] = expression->json();
	return root;
}

void Return::gen(Context &context) {
	if (expression)
		addDebugLoc(context, context.getBuilder().CreateRet(expression->load(context)), loc);
	else
		addDebugLoc(context, context.getBuilder().CreateRetVoid(), loc);
	context.newBlock();
}
