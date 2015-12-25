#include "MemberVariableDefination.h"
#include "Class.h"
#include "Qualifier.h"
#include "VariableDefination.h"
#include "Symbol.h"
#include "Identifier.h"
#include "Type.h"
#include "util.h"

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
	if (qualifier->isStatic())
		for (VariableDefination::iterator it = vars->begin(); it != vars->end(); it++) {
			llvm::Value *old = cls->symbols.find(it->first->getName())->data.identifier.value;
			llvm::Value *_new = new llvm::GlobalVariable(context.getModule(), vars->type->getType(context), qualifier->isConst(), llvm::GlobalVariable::ExternalLinkage, vars->type->getDefault(context));
			old->replaceAllUsesWith(_new);
			((llvm::GlobalVariable *) old)->eraseFromParent();
			cls->symbols.find(it->first->getName())->data.identifier.value = _new;
			_new->setName(cls->getMangleName() + "S" + itos(it->first->getName().length()) + it->first->getName());
		}
}

void MemberVariableDefination::genStruct(Context &context) {
	if (!qualifier->isStatic())
		for (VariableDefination::iterator it = vars->begin(); it != vars->end(); it++) {
			cls->members.push_back(vars->type->getType(context));
			cls->symbols.add(new Symbol(it->first->getName(), qualifier, vars->type, cls->members.size() - 1));
		}
	else
		for (VariableDefination::iterator it = vars->begin(); it != vars->end(); it++)
			cls->symbols.add(new Symbol(it->first->getName(), qualifier, vars->type, new llvm::GlobalVariable(context.getModule(), vars->type->getType(context), qualifier->isConst(), llvm::GlobalVariable::ExternalLinkage, NULL, cls->getMangleName() + "S" + itos(it->first->getName().length()) + it->first->getName())));
}
