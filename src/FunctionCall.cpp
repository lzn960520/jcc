#include "FunctionCall.h"
#include "exception.h"
#include "Context.h"
#include "Identifier.h"
#include "Function.h"
#include "Symbol.h"
#include "Type.h"
#include "Class.h"
#include "DebugInfo.h"
#include <iostream>

FunctionCall::FunctionCall(Expression *target, Identifier *identifier, std::list<Expression*> *arg_list) :
	target(target), identifier(identifier), arg_list(*arg_list) {
}

FunctionCall::~FunctionCall() {
	if (target)
		delete target;
	delete identifier;
	delete &arg_list;
}

Json::Value FunctionCall::json() {
	Json::Value root;
	root["name"] = "call";
	if (target)
		root["target"] = target->json();
	root["identifier"] = identifier->json();
	root["arg_list"] = Json::Value(Json::arrayValue);
	int i = 0;
	for (std::list<Expression*>::iterator it = arg_list.begin(); it != arg_list.end(); it++, i++)
		root["arg_list"][i] = (*it)->json();
	return root;
}

llvm::Value* FunctionCall::load(Context &context) {
	Class *targetClass = NULL;
	std::string funcName = identifier->getName();
	if (target == NULL) {
		if (identifier->getName().rfind("::") != std::string::npos) {
			targetClass = context.findClass(identifier->getName().substr(0, identifier->getName().rfind("::")));
			funcName = identifier->getName().substr(identifier->getName().rfind("::") + 2);
			if (!targetClass)
				throw SymbolNotFound(identifier->getName().substr(0, identifier->getName().rfind("::")));
		} else
			targetClass = context.currentFunction->getClass();
	} else
		targetClass = target->getType(context)->getClass();
	Symbol *symbol = targetClass->findSymbol(funcName);
	if (!symbol) {
		std::cout << *targetClass;
		throw FunctionNotFound(identifier->getName());
	}

	llvm::Value *function;
	std::vector<llvm::Value*> arg_code;
	switch (symbol->type) {
	case Symbol::FUNCTION: {
		Expression *tmpTarget = target ? target : new Identifier("this");
		if (!tmpTarget->getType(context)->isObject())
			throw InvalidType(std::string("calling a function of ") + tmpTarget->getType(context)->getName());
		llvm::Value *thisval = tmpTarget->load(context);
		if (!target)
			delete tmpTarget;

		if (targetClass->getMangleName()[0] == 'I') {
			function = addDebugLoc(
					context,
					context.getBuilder().CreateLoad(
							thisval
					),
					loc
			);
			// for interface call, we have to recalculate the object base address
			llvm::Value *baseOffset = addDebugLoc(
					context,
					context.getBuilder().CreateLoad(
							addDebugLoc(
									context,
									context.getBuilder().CreateStructGEP(
											nullptr,
											function,
											0
									),
									loc
							)
					),
					loc
			);
			thisval = addDebugLoc(
					context,
					context.getBuilder().CreateIntToPtr(
							addDebugLoc(
									context,
									context.getBuilder().CreateSub(
											addDebugLoc(
													context,
													context.getBuilder().CreatePtrToInt(
															thisval,
															context.getBuilder().getInt32Ty()
													),
													loc
											),
											baseOffset
									),
									loc
							),
							context.getBuilder().getInt8PtrTy(0)
					),
					loc
			);
		} else {
			function = addDebugLoc(
					context,
					context.getBuilder().CreateLoad(
							addDebugLoc(
									context,
									context.getBuilder().CreateStructGEP(
											nullptr,
											thisval,
											symbol->data.function.vtableOffset
									),
									loc
							)
					),
					loc
			);
			thisval = context.getBuilder().CreatePointerCast(
					thisval,
					context.getBuilder().getInt8PtrTy(0)
			);
		}
		arg_code.push_back(thisval);
		function = addDebugLoc(
				context,
				context.getBuilder().CreateLoad(
						addDebugLoc(
								context,
								context.getBuilder().CreateStructGEP(
										nullptr,
										function,
										symbol->data.function.funcPtrOffset
								),
								loc
						)
				),
				loc
		);
		break; }
	case Symbol::STATIC_FUNCTION:
		function = symbol->data.static_function.function->getLLVMFunction(context);
		if (target)
			throw CompileException("Calling a static function of an object");
		break;
	default:
		throw InvalidType("calling a symbol which is not a function");
	}
	if (symbol->type == Symbol::STATIC_FUNCTION) {
		// Calling a static function
	} else {
	}
	for (std::list<Expression*>::iterator it = arg_list.begin(); it != arg_list.end(); it++)
		arg_code.push_back((*it)->load(context));
	llvm::Value *ans = addDebugLoc(
			context,
			context.getBuilder().CreateCall(function, llvm::ArrayRef<llvm::Value*>(arg_code)),
			loc);
	return ans;
}

llvm::Instruction* FunctionCall::store(Context &context, llvm::Value *value) {
	throw NotAssignable("function call");
}

Type* FunctionCall::getType(Context &context) {
	Symbol *symbol = NULL;
	if (target)
		symbol = target->getType(context)->getClass()->findSymbol(identifier->getName());
	else if (identifier->getName().rfind("::") != std::string::npos) {
		Class *targetClass = context.findClass(identifier->getName().substr(0, identifier->getName().rfind("::")));
		if (!targetClass)
			throw SymbolNotFound(identifier->getName().substr(0, identifier->getName().rfind("::")));
		symbol = targetClass->findSymbol(identifier->getName().substr(identifier->getName().rfind("::") + 2));
	} else
		symbol = context.currentClass->findSymbol(identifier->getName());
	if (!symbol)
		throw SymbolNotFound(identifier->getName());
	switch (symbol->type) {
	case Symbol::STATIC_FUNCTION:
		return symbol->data.static_function.function->getReturnType();
	case Symbol::FUNCTION:
	default:
		throw InvalidType("calling a symbol which is not a function");
	}
}

Expression::Constant FunctionCall::loadConstant() {
	throw NotConstant("function call");
}

Type* FunctionCall::getTypeConstant() {
	throw NotConstant("function call");
}
