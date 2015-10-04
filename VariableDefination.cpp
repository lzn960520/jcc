#include "yyvaltypes.h"
#include <llvm/IR/ValueSymbolTable.h>

VariableDefination::VariableDefination(ASTNode *type) :
	type(dynamic_cast<Type*>(type)) {
}

void VariableDefination::push_back(ASTNode *identifier) {
	list.push_back(std::pair<Identifier*,ASTNode*>(dynamic_cast<Identifier*>(identifier), NULL));
}

void VariableDefination::push_back(ASTNode *identifier, ASTNode *expression) {
	list.push_back(std::pair<Identifier*,ASTNode*>(dynamic_cast<Identifier*>(identifier), expression));
}

VariableDefination::~VariableDefination() {
	for (std::list<std::pair<Identifier*,ASTNode*> >::iterator it = list.begin(); it != list.end(); it++) {
		if (it->first)
			delete it->first;
		if (it->second)
			delete it->second;
	}
	if (type)
		delete type;
}

Json::Value VariableDefination::json() {
	Json::Value root;
	root["name"] = "variable_defination";
	root["type"] = type->json();
	root["list"] = Json::Value(Json::arrayValue);
	int i = 0;
	for (std::list<std::pair<Identifier*,ASTNode*> >::iterator it = list.begin(); it != list.end(); it++, i++) {
		root["list"][i] = Json::Value();
		root["list"][i]["identifier"] = it->first->json();
		if (it->second)
			root["list"][i]["init_value"] = it->second->json();
	}
	return root;
}

void *VariableDefination::gen(Context &context) {
	llvm::Type *type = this->type->getType(context);
	for (std::list<std::pair<Identifier*,ASTNode*> >::iterator it = list.begin(); it != list.end(); it++) {
		llvm::AllocaInst *tmp = context.getBuilder().CreateAlloca(type, NULL, it->first->getName());
		if (it->second)
			context.getBuilder().CreateStore((llvm::Value *) it->second->gen(context), tmp, false);
		context.addSymbol(it->first->getName(), tmp);
	}
	return NULL;
}
