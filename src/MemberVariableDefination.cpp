#include "MemberVariableDefination.h"
#include "Class.h"
#include "Qualifier.h"
#include "VariableDefination.h"
#include "Symbol.h"
#include "Identifier.h"
#include "Type.h"

MemberVariableDefination::MemberVariableDefination(Qualifier *qualifier, VariableDefination *vars) :
		qualifier(qualifier), vars(vars) {
}

MemberVariableDefination::~MemberVariableDefination() {
}

Json::Value MemberVariableDefination::json() {
	Json::Value root;
	root["name"] = "member_var";
	root["qualifier"] = qualifier->json();
	root["vars"] = vars->json();
	return root;
}

void MemberVariableDefination::gen(Context &context) {
	for (VariableDefination::iterator it = vars->begin(); it != vars->end(); it++)
		context.addSymbol(new Symbol(it->first->getName(), vars->type, cls->symbols.find(it->first->getName())->data.member.index));
}

void MemberVariableDefination::genStruct(Context &context) {
	for (VariableDefination::iterator it = vars->begin(); it != vars->end(); it++) {
		cls->members.push_back(vars->type->getType(context));
		cls->symbols.add(new Symbol(it->first->getName(), vars->type, cls->members.size() - 1));
	}
}
