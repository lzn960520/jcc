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

llvm::Value* MemberAccess::getPointer(Context &context) {
	if (isStatic) {
		// find class
		Class *cls = targetClass->getClass(context);
		if (!cls)
			throw SymbolNotFound("No such class '" + targetClass->getName() + "'");
		if (cls->getMangleName()[0] == 'J')
			throw InvalidType("'" + cls->getFullName() + "' is an interface");
		// find symbol
		Symbol *symbol = cls->findSymbol(identifier->getName());
		if (!symbol)
			throw SymbolNotFound("No such static member '" + identifier->getName() + "' in class '" + cls->getFullName() + "'");
		if (symbol->type != Symbol::STATIC_MEMBER_VAR)
			throw InvalidType("'" + identifier->getName() + "' in '" + targetClass->getName() + "' is not a static member variable");
		// check permission
		if (symbol->data.identifier.isPrivate && cls != context.currentClass)
			throw CompileException("'" + identifier->getName() + "' is private");
		if (symbol->data.identifier.isProtected && !Class::isA(context.currentClass, cls))
			throw CompileException("'" + identifier->getName() + "' is protected");
		return symbol->data.identifier.value;
	} else {
		// find class
		if (!target->getType(context)->isObject())
			throw InvalidType(std::string("Access member of a ") + target->getType(context)->getName());
		Class *cls = target->getType(context)->getClass();
		// find symbol
		Symbol *symbol = cls->findSymbol(identifier->getName());
		if (!symbol)
			throw SymbolNotFound(std::string("No such member variable in class ") + cls->getName());
		if (symbol->type != Symbol::MEMBER_VAR)
			throw InvalidType("'" + identifier->getName() + "' in '" + targetClass->getName() + "' is not a member variable");
		// check permission
		if (symbol->data.identifier.isPrivate && cls != context.currentClass)
			throw CompileException("'" + identifier->getName() + "' is private");
		if (symbol->data.identifier.isProtected && !Class::isA(context.currentClass, cls))
			throw CompileException("'" + identifier->getName() + "' is protected");
		return addDebugLoc(
				context,
				context.getBuilder().CreateStructGEP(
						nullptr,
						target->load(context),
						symbol->data.member.index),
				loc);
	}
}

Type* MemberAccess::getType(Context &context) {
	if (isStatic) {
		Class *cls = targetClass->getClass(context);
		if (!cls)
			throw SymbolNotFound("No such class '" + targetClass->getName() + "'");
		if (cls->getMangleName()[0] == 'J')
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

llvm::Value* MemberAccess::load(Context &context) {
	llvm::Value *ptr = getPointer(context);
	if (ptr->getType()->getPointerElementType()->isArrayTy())
		return ptr;
	else
		return addDebugLoc(
				context,
				context.getBuilder().CreateLoad(ptr),
				loc
		);
}

llvm::Instruction* MemberAccess::store(Context &context, llvm::Value *value) {
	llvm::Value *ptr = getPointer(context);
	if (ptr->getType()->isArrayTy())
		throw NotAssignable("member array");
	else
		return context.getBuilder().CreateStore(
				value,
				ptr
		);
}

Expression::Constant MemberAccess::loadConstant() {
	throw NotConstant("member of object");
}

Type* MemberAccess::getTypeConstant() {
	throw NotConstant("member of object");
}
