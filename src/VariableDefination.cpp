#include "VariableDefination.h"
#include "Context.h"
#include "Identifier.h"
#include "Type.h"
#include "Expression.h"
#include "Symbol.h"
#include "exception.h"
#include "DebugInfo.h"

VariableDefination::VariableDefination(Type *type, std::list<std::pair<Identifier*, Expression*> > *list) :
	type(type), list(*list) {
}

VariableDefination::~VariableDefination() {
	delete type;
	delete &list;
}

Json::Value VariableDefination::json() {
	Json::Value root;
	root["name"] = "variable_defination";
	root["type"] = type->json();
	root["list"] = Json::Value(Json::arrayValue);
	int i = 0;
	for (std::list<std::pair<Identifier *, Expression *> >::iterator it = list.begin(); it != list.end(); i++, it++) {
		root["list"][i] = Json::Value();
		root["list"][i]["identifier"] = it->first->json();
		if (it->second)
			root["list"][i]["init_value"] = it->second->json();
	}
	return root;
}

void VariableDefination::gen(Context &context) {
	if (this->type->isArray()) {
		if (this->type->internal->isArray())
			throw NotImplemented("nested array");
		size_t totalSize = 1;
		for (std::vector<std::pair<int, int> >::iterator it = this->type->arrayDim.begin(); it != this->type->arrayDim.end(); it++) {
			if (it->first >= it->second)
				throw NotImplemented("dynamic array");
			totalSize = totalSize * (it->second - it->first + 1);
		}
		for (std::list<std::pair<Identifier*, Expression*> >::iterator it = list.begin(); it != list.end(); it++) {
			llvm::AllocaInst *tmp = addDebugLoc(
					context,
					context.getBuilder().CreateAlloca(
							this->type->internal->getType(context),
							llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), totalSize, false),
							it->first->getName()),
					it->first->loc);
			insertDeclareDebugInfo(context, this->type, it->first->getName(), tmp, it->first->loc, false);
			if (it->second)
				throw NotImplemented("initial array");
			context.addSymbol(new Symbol(it->first->getName(), Symbol::LOCAL_VAR, this->type, tmp));
		}
	} else if (this->type->isObject()) {
		llvm::Type *type = llvm::PointerType::get(this->type->getType(context), 0);
		for (std::list<std::pair<Identifier*, Expression*> >::iterator it = list.begin(); it != list.end(); it++) {
			llvm::AllocaInst *tmp = addDebugLoc(context, context.getBuilder().CreateAlloca(type, NULL, it->first->getName()), it->first->loc);
			insertDeclareDebugInfo(context, this->type, it->first->getName(), tmp, it->first->loc, false);
			if (it->second)
				addDebugLoc(context, context.getBuilder().CreateStore(it->second->load(context), tmp, false), it->first->loc);
			context.addSymbol(new Symbol(it->first->getName(), Symbol::LOCAL_VAR, this->type, tmp));
		}
	} else {
		llvm::Type *type = this->type->getType(context);
		for (std::list<std::pair<Identifier*, Expression*> >::iterator it = list.begin(); it != list.end(); it++) {
			llvm::AllocaInst *tmp = addDebugLoc(context, context.getBuilder().CreateAlloca(type, NULL, it->first->getName()), it->first->loc);
			insertDeclareDebugInfo(context, this->type, it->first->getName(), tmp, it->first->loc, false);
			if (it->second)
				addDebugLoc(context, context.getBuilder().CreateStore(it->second->load(context), tmp, false), it->first->loc);
			context.addSymbol(new Symbol(it->first->getName(), Symbol::LOCAL_VAR, this->type, tmp));
		}
	}
}
