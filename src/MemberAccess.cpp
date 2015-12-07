#include "MemberAccess.h"
#include "Type.h"
#include "Context.h"
#include "exception.h"
#include "Class.h"
#include "Identifier.h"
#include "DebugInfo.h"
#include "Symbol.h"

MemberAccess::MemberAccess(Expression *target, Identifier *identifier) :
		target(target), targetClass(NULL), identifier(identifier), isStatic(false) {
}

MemberAccess::MemberAccess(Type *cls, Identifier *identifier) :
		target(NULL), targetClass(cls), identifier(identifier), isStatic(true) {
}

MemberAccess::~MemberAccess() {
	if (target)
		delete target;
	if (targetClass)
		delete targetClass;
	delete identifier;
}

Json::Value MemberAccess::json() {
	Json::Value ans;
	ans["name"] = "member_access";
	ans["isStatic"] = isStatic;
	ans["identifier"] = identifier->json();
	if (!isStatic)
		ans["target"] = target->json();
	else
		ans["target"] = targetClass->json();
	return ans;
}

llvm::Value* MemberAccess::load(Context &context) {
	if (isStatic) {
		Class *cls = targetClass->getClass(context);
		if (!cls)
			throw SymbolNotFound("No such class '" + targetClass->getName() + "'");
		if (cls->getMangleName()[0] == 'I')
			throw InvalidType("'" + cls->getFullName() + "' is an interface");
		Symbol *symbol = cls->findSymbol(identifier->getName());
		if (!symbol)
			throw SymbolNotFound("No such static member '" + identifier->getName() + "' in class '" + cls->getFullName() + "'");
		return addDebugLoc(
				context,
				context.getBuilder().CreateLoad(
						symbol->data.identifier.value
				),
				loc);
	} else {
		if (!target->getType(context)->isObject())
			throw InvalidType(std::string("Access member of a ") + target->getType(context)->getName());
		Class *cls = target->getType(context)->getClass();
		Symbol *symbol = cls->findSymbol(identifier->getName());
		if (!symbol)
			throw SymbolNotFound(std::string("No such member variable in class ") + cls->getName());
		llvm::Value *tmp = addDebugLoc(
				context,
				context.getBuilder().CreateStructGEP(
						nullptr,
						target->load(context),
						symbol->data.member.index),
				loc);
		if (tmp->getType()->getPointerElementType()->isArrayTy())
			return tmp;
		else
			return addDebugLoc(
					context,
					context.getBuilder().CreateLoad(tmp),
					loc);
	}
}

Type* MemberAccess::getType(Context &context) {
	if (isStatic) {
		Class *cls = targetClass->getClass(context);
		if (!cls)
			throw SymbolNotFound("No such class '" + targetClass->getName() + "'");
		if (cls->getMangleName()[0] == 'I')
			throw InvalidType("'" + cls->getFullName() + "' is an interface");
		Symbol *symbol = cls->findSymbol(identifier->getName());
		if (!symbol)
			throw SymbolNotFound("No such static member '" + identifier->getName() + "' in class '" + cls->getFullName() + "'");
		return symbol->data.identifier.type;
	} else {
		if (!target->getType(context)->isObject())
			throw InvalidType(std::string("Access member of a ") + target->getType(context)->getName());
		Class *cls = target->getType(context)->getClass();
		Symbol *symbol = cls->findSymbol(identifier->getName());
		if (!symbol)
			throw SymbolNotFound(std::string("No such member variable in class ") + cls->getName());
		return symbol->data.member.type;
	}
}

llvm::Instruction* MemberAccess::store(Context &context, llvm::Value *value) {
	if (isStatic) {
		Class *cls = targetClass->getClass(context);
		if (!cls)
			throw SymbolNotFound("No such class '" + targetClass->getName() + "'");
		if (cls->getMangleName()[0] == 'I')
			throw InvalidType("'" + cls->getFullName() + "' is an interface");
		Symbol *symbol = cls->findSymbol(identifier->getName());
		if (!symbol)
			throw SymbolNotFound("No such static member '" + identifier->getName() + "' in class '" + cls->getFullName() + "'");
		return context.getBuilder().CreateStore(
				value,
				symbol->data.identifier.value
		);
	} else {
		if (!target->getType(context)->isObject())
			throw InvalidType(std::string("Access member of a ") + target->getType(context)->getName());
		Class *cls = target->getType(context)->getClass();
		Symbol *symbol = cls->findSymbol(identifier->getName());
		if (!symbol)
			throw SymbolNotFound(std::string("No such member variable in class ") + cls->getName());
		llvm::Value *tmp = addDebugLoc(
				context,
				context.getBuilder().CreateStructGEP(
						nullptr,
						target->load(context),
						symbol->data.member.index),
				loc);
		if (tmp->getType()->isArrayTy())
			throw NotAssignable("member array");
		else
			return addDebugLoc(
					context,
					context.getBuilder().CreateStore(value, tmp),
					loc);
	}
}

Expression::Constant MemberAccess::loadConstant() {
	throw NotConstant("member of object");
}

Type* MemberAccess::getTypeConstant() {
	throw NotConstant("member of object");
}
