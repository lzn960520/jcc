#include <llvm/IR/Type.h>

#include "Type.h"
#include "exception.h"
#include "Context.h"
#include "ArrayDefinator.h"
#include "Expression.h"
#include "Class.h"
#include "Identifier.h"
#include "Symbol.h"
#include "Namespace.h"
#include "DebugInfo.h"

const char *Type::BaseTypeNames[] = {
	"byte",
	"short",
	"int",
	"char",
	"float",
	"double",
	"array",
	"bool",
	"string",
	"object"
};

Type Type::Bool(Type::BOOL);
Type Type::String(Type::STRING);
Type Type::Int32(Type::INT);
Type Type::UInt32(Type::INT, true);

Type::Type(BaseType baseType, bool isUnsigned) :
			baseType(baseType), isUnsigned(isUnsigned), internal(NULL), cls(NULL), identifier(NULL) {
	assert(baseType != ARRAY);
	if ((baseType == CHAR || baseType == FLOAT || baseType == DOUBLE) && isUnsigned)
		throw InvalidType("Can't set char, float or double to unsigned");
}

Type::Type(BaseType array, Type *baseType, ArrayDefinator *definator) :
			baseType(array), isUnsigned(false), internal(baseType), cls(NULL), identifier(NULL) {
	assert(array == ARRAY);
	for (std::vector<std::pair<Expression *, Expression *> >::iterator it = definator->list.begin(); it != definator->list.end(); it++) {
		if (it->first && !it->first->isConstant())
			throw InvalidType("Dim expression must be constant");
		if (it->second && !it->second->isConstant())
			throw InvalidType("Dim expression must be constant");
		if (!it->first)
			throw NotImplemented("dynamic array");
		if (!it->second)
			throw NotImplemented("dynamic array");
		if (it->first->getTypeConstant()->isFloat() || it->second->getTypeConstant()->isFloat())
			throw InvalidType("Dim expression must be integer");
		arrayDim.push_back(std::pair<unsigned int, unsigned int>(it->first->loadConstant()._uint64, it->second->loadConstant()._uint64));
	}
}

Type::Type(Class *cls) :
		baseType(OBJECT), cls(cls), internal(NULL), isUnsigned(false), identifier(NULL) {
}

Type::Type(Identifier *identifier) :
		baseType(OBJECT), cls(NULL), internal(NULL), isUnsigned(false), identifier(identifier) {
}

Type::~Type() {
	if (identifier)
		delete identifier;
	if (internal)
		delete internal;
}

Json::Value Type::json() {
	Json::Value root;
	root["name"] = "type";
	root["base_type"] = BaseTypeNames[baseType];
	switch (baseType) {
	case ARRAY: {
		root["internal"] = internal->json();
		root["dim"] = Json::Value(Json::arrayValue);
		int i = 0;
		for (std::vector<std::pair<int, int> >::iterator it = arrayDim.begin(); it != arrayDim.end(); it++, i++) {
			root["dim"][i] = Json::Value();
			if (it->first > it->second) {
				root["dim"][i]["lower"] = "dym";
				root["dim"][i]["upper"] = "dym";
			} else if (it->first == it->second) {
				root["dim"][i]["lower"] = it->first;
				root["dim"][i]["upper"] = "dym";
			} else {
				root["dim"][i]["lower"] = it->first;
				root["dim"][i]["upper"] = it->second;
			}
		}
		break;
	}
	case OBJECT:
		if (cls)
			root["class"] = cls->getPrefix().substr(0, cls->getPrefix().length() - 1);
		else
			root["class"] = identifier->getName();
		break;
	case BYTE:
	case SHORT:
	case INT:
		root["is_unsigned"] = isUnsigned;
		break;
	}
	return root;
}

llvm::Type* Type::getType(Context &context) {
	switch (baseType) {
	case BYTE:
		return context.getBuilder().getInt8Ty();
	case SHORT:
		return context.getBuilder().getInt16Ty();
	case INT:
		return context.getBuilder().getInt32Ty();
	case ARRAY:
		{
			size_t totalSize = 1;
			for (std::vector<std::pair<int, int> >::iterator it = arrayDim.begin(); it != arrayDim.end(); it++) {
				if (it->first >= it->second)
					throw NotImplemented("dynamic array");
				totalSize *= it->second - it->first;
			}
			return llvm::ArrayType::get(internal->getType(context), totalSize);
		}
		break;
	case OBJECT:
		if (!cls) {
			Symbol *sym = context.findSymbol(context.currentNS->getPrefix() + identifier->getName());
			if (!sym)
				throw SymbolNotFound("No such class: " + context.currentNS->getPrefix() + identifier->getName());
			if (sym->type != Symbol::CLASS)
				throw InvalidType(context.currentNS->getPrefix() + identifier->getName() + " is not a class");
			cls = sym->data.cls.cls;
		}
		return cls->getLLVMType();
	}
}

size_t Type::getSize() {
	switch (baseType) {
	case BYTE:
		return 1;
	case SHORT:
		return 2;
	case INT:
		return 4;
	case OBJECT:
		throw NotImplemented("caculate size of object");
	}
}

Type* Type::higherType(Type *a, Type *b) {
	if (a->isArray() || b->isArray())
		throw IncompatibleType(BaseTypeNames[a->baseType], BaseTypeNames[b->baseType]);
	if ((a->isChar() && b->isString()) || (b->isChar() && a->isString()) || (a->isChar() && b->isChar()))
		return &Type::String;
	if (a->baseType == b->baseType)
		return a;
	if (a->baseType == DOUBLE)
		return a;
	if (b->baseType == DOUBLE)
		return b;
	if (a->baseType == FLOAT)
		return a;
	if (b->baseType == FLOAT)
		return b;
	if (a->baseType == INT && a->isUnsigned)
		return a;
	if (b->baseType == INT && b->isUnsigned)
		return b;
	if (a->baseType == INT)
		return a;
	if (b->baseType == INT)
		return b;
	if (a->baseType == SHORT && a->isUnsigned)
		return a;
	if (b->baseType == SHORT && b->isUnsigned)
		return b;
	if (a->baseType == SHORT)
		return a;
	if (b->baseType == SHORT)
		return b;
	if (a->baseType == BYTE && a->isUnsigned)
		return a;
	if (b->baseType == BYTE && b->isUnsigned)
		return b;
	if (a->baseType == BYTE)
		return a;
	if (b->baseType == BYTE)
		return b;
	throw IncompatibleType(BaseTypeNames[a->baseType], BaseTypeNames[b->baseType]);
}

llvm::Value* Type::cast(Context &context, Type *otype, llvm::Value *val, Type *dtype) {
	switch (dtype->baseType) {
	case INT:
	case SHORT:
	case BYTE:
	case BOOL:
		if (otype->isInt())
			if (otype->getSize() == dtype->getSize())
				return val;
			else
				return context.getBuilder().CreateTruncOrBitCast(val, dtype->getType(context));
		else if (otype->isFloat())
			return context.getBuilder().CreateFPCast(val, dtype->getType(context));
		break;
	case FLOAT:
	case DOUBLE:
		if (otype->isNumber())
			return context.getBuilder().CreateFPCast(val, dtype->getType(context));
		break;
	case CHAR:
	case STRING:
		break;
	case OBJECT:
	case ARRAY:
		break;
	}
	throw IncompatibleType(std::string("convert ") + BaseTypeNames[otype->baseType] + " to " + BaseTypeNames[dtype->baseType]);
}
