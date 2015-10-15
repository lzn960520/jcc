#include "Type.h"
#include "exception.h"
#include "Context.h"
#include "ArrayDefinator.h"
#include "Expression.h"

const char *Type::BaseTypeNames[] = {
	"byte",
	"short",
	"int",
	"char",
	"float",
	"double",
	"array",
	"bool",
	"string"
};

Type Type::Bool(Type::BOOL);
Type Type::String(Type::STRING);
Type Type::Int32(Type::INT);
Type Type::UInt32(Type::INT, true);

Type::Type(BaseType baseType, bool isUnsigned) :
	baseType(baseType), isUnsigned(isUnsigned), internal(NULL) {
	assert(baseType != ARRAY);
	if ((baseType == CHAR || baseType == FLOAT || baseType == DOUBLE) && isUnsigned)
		throw InvalidType("Can't set char, float or double to unsigned");
}

Type::Type(BaseType array, Type *baseType, ArrayDefinator *definator) :
	baseType(array), isUnsigned(false), internal(baseType) {
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

Type::~Type() {
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
	default:
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
