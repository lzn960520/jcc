#include "VariableDefination.h"
#include "Context.h"
#include "Identifier.h"
#include "Type.h"
#include "Expression.h"
#include "Symbol.h"
#include "exception.h"
#include "DebugInfo.h"
#include "New.h"
#include "Op2.h"
#include "ArrayAccessor.h"

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
		// check before
		for (std::vector<std::pair<Expression*, Expression*> >::const_iterator it = type->arrayDim.begin(); it != type->arrayDim.end(); it++)
			if (!it->first->getTypeConstant()->isInt() || (it->second && !it->second->getTypeConstant()->isInt()))
				throw InvalidType("dim expression must be integer");
		// determine whether it is an array pointer
		for (std::vector<std::pair<Expression*, Expression*> >::const_iterator it = type->arrayDim.begin(); it != type->arrayDim.end(); it++)
			if (!it->second) {
				// this is an array pointer
				for (iterator it = begin(); it != end(); it++) {
					llvm::Value *alloca = addDebugLoc(
							context,
							context.getBuilder().CreateAlloca(
									type->getType(context),
									nullptr,
									it->first->getName()),
							loc);
					insertDeclareDebugInfo(context, type, it->first->getName(), alloca, loc, false);
					Op2 *assign = new Op2(it->first->clone(), Op2::ASSIGN, it->second);
					try {
						assign->load(context);
					} catch (...) {
						delete assign;
						throw;
					}
					delete assign;
				}
				return;
			}
		// generate template New
		New *_template = NULL;
		{
			ArrayAccessor *accessor = new ArrayAccessor(new Op2(type->arrayDim[0].second, Op2::SUB, type->arrayDim[0].first));
			for (size_t i = 1, len = type->arrayDim.size(); i != len; i++)
				accessor->push_back(new Op2(type->arrayDim[i].second, Op2::SUB, type->arrayDim[i].first));
			_template = new New(type->internal, accessor);
		}
		// generate alloca and assign
		{
			for (iterator it = begin(); it != end(); it++) {
				llvm::Value *alloca = addDebugLoc(
						context,
						context.getBuilder().CreateAlloca(
								type->getType(context),
								nullptr,
								it->first->getName()),
						loc);
				insertDeclareDebugInfo(context, type, it->first->getName(), alloca, loc, false);
				context.addSymbol(new Symbol(it->first->getName(), Symbol::LOCAL_VAR, type, alloca));
				addDebugLoc(
						context,
						context.getBuilder().CreateStore(
								_template->load(context),
								alloca),
						loc);
				if (it->second) {
					delete _template;
					throw NotImplemented("array initializer");
				}
			}
		}
		delete _template;
	} else {
		for (iterator it = begin(); it != end(); it++) {
			llvm::Value *alloca = addDebugLoc(
					context,
					context.getBuilder().CreateAlloca(
							type->getType(context),
							nullptr,
							it->first->getName()),
					loc);
			insertDeclareDebugInfo(context, type, it->first->getName(), alloca, loc, false);
			context.addSymbol(new Symbol(it->first->getName(), Symbol::LOCAL_VAR, type, alloca));
		}
	}
}
