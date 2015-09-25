#include "yyvaltypes.h"
#include "exception.h"

const char *Type::BaseTypeNames[] = {
	"byte",
	"short",
	"int",
	"char",
	"float",
	"double"
};

Type::Type(BaseType baseType, bool isUnsigned) :
	baseType(baseType), isUnsigned(isUnsigned) {
	if ((baseType == CHAR || baseType == FLOAT || baseType == DOUBLE) && isUnsigned)
		throw InvalidType("Can't set char, float or double to unsigned");
}

Type::~Type() {
}

Json::Value Type::json() {
	Json::Value root;
	root["name"] = "type";
	root["base_type"] = BaseTypeNames[baseType];
	root["is_unsigned"] = isUnsigned;
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
