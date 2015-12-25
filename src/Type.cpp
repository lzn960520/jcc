#include <llvm/IR/Type.h>

#include "Type.h"
#include "exception.h"
#include "Context.h"
#include "ArrayDefinator.h"
#include "Expression.h"
#include "Class.h"
#include "Identifier.h"
#include "Symbol.h"
#include "DebugInfo.h"
#include "Module.h"
#include "util.h"
#include "LiteralInt.h"

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
Type Type::String(new Identifier("string"), 0);
Type Type::Int32(Type::INT);
Type Type::UInt32(Type::INT, true);

Type::Type(BaseType baseType, bool isUnsigned) :
			baseType(baseType), isUnsigned(isUnsigned), internal(NULL), cls(NULL), identifier(NULL) {
	assert(baseType != ARRAY);
	if ((baseType == CHAR || baseType == FLOAT || baseType == DOUBLE) && isUnsigned)
		throw InvalidType("Can't set char, float or double to unsigned");
}

Type::Type(Type *baseType, ArrayDefinator *definator) :
			baseType(ARRAY), isUnsigned(false), internal(baseType), cls(NULL), identifier(NULL), arrayDim(definator->list) {
	definator->list.clear();
	delete definator;
}

Type::Type(Type *baseType, const std::vector<std::pair<int, int> > &dims) :
			baseType(ARRAY), isUnsigned(false), internal(baseType), cls(NULL), identifier(NULL) {
	for (std::vector<std::pair<int, int> >::const_iterator it = dims.begin(); it != dims.end(); it++)
		const_cast<std::vector<std::pair<Expression*, Expression*> >*>(&arrayDim)->push_back(std::pair<Expression*, Expression*>(new LiteralInt(it->first, true), new LiteralInt(it->second, true)));
}

Type::Type(Class *cls) :
		baseType(OBJECT), cls(cls), internal(NULL), isUnsigned(false), identifier(NULL) {
}

Type::Type(Identifier *identifier, int) :
		baseType(OBJECT), cls(NULL), internal(NULL), isUnsigned(false), identifier(identifier) {
}

Type::Type(Identifier *identifier) :
		baseType(OBJECT), cls(NULL), internal(NULL), isUnsigned(false), identifier(identifier) {
	if (identifier->getName() == "string") {
		*const_cast<enum BaseType*>(&baseType) = STRING;
		delete identifier;
		*const_cast<Identifier**>(&this->identifier) = NULL;
	}
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
		for (std::vector<std::pair<Expression*, Expression*> >::const_iterator it = arrayDim.begin(); it != arrayDim.end(); it++, i++) {
			root["dim"][i] = Json::Value();
			root["dim"][i]["lower"] = it->first->json();
			if (it->second)
				root["dim"][i]["upper"] = it->second->json();
			else
				root["dim"][i]["upper"] = "dynamic";
		}
		break;
	}
	case OBJECT:
		if (cls)
			root["class"] = cls->getFullName();
		else
			root["class"] = identifier->getName();
		break;
	case BYTE:
	case SHORT:
	case INT:
		root["is_unsigned"] = isUnsigned;
		break;
	case STRING:
	case CHAR:
		break;
	}
	return root;
}

llvm::Type* Type::getType(Context &context) {
	switch (baseType) {
	case BOOL:
		return context.getBuilder().getInt1Ty();
	case BYTE:
		return context.getBuilder().getInt8Ty();
	case SHORT:
		return context.getBuilder().getInt16Ty();
	case INT:
		return context.getBuilder().getInt32Ty();
	case FLOAT:
		return context.getBuilder().getFloatTy();
	case DOUBLE:
		return context.getBuilder().getDoubleTy();
	case ARRAY:
		{
			size_t totalSize = 1;
			for (std::vector<std::pair<Expression*, Expression*> >::const_iterator it = arrayDim.begin(); it != arrayDim.end(); it++) {
				if (!it->second)
					throw NotImplemented("dynamic array");
				if (!it->first->isConstant() || !it->second->isConstant())
					throw NotImplemented("dynamic dim expression");
				if (!it->first->getTypeConstant()->isInt() || !it->second->getTypeConstant()->isInt())
					throw InvalidType("dim expression must be integer");
				totalSize *= it->second->loadConstant()._int64 - it->first->loadConstant()._int64 + 1;
			}
			return llvm::ArrayType::get(internal->getType(context), totalSize);
		}
		break;
	case OBJECT:
		if (!cls)
			*const_cast<Class**>(&cls) = context.findClass(identifier->getName());
		if (!cls)
			throw SymbolNotFound("No such class '" + context.currentModule->getFullName() + "::" + identifier->getName() + "'");
		return llvm::PointerType::get(cls->getLLVMType(), 0);
	case STRING:
		return String.getType(context);
	}
	throw NotImplemented(std::string("type '") + getName() + "'");
}

size_t Type::getSize(Context &context) {
	switch (baseType) {
	case BOOL:
	case BYTE:
	case SHORT:
	case INT:
		return context.DL->getTypeAllocSize(getType(context));
	case OBJECT: {
		return context.DL->getTypeAllocSize(getType(context)->getPointerElementType()); }
	case STRING:
		return String.getSize(context);
	default:
		throw NotImplemented("type '" + getName() + "'");
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

bool Type::isA(Type *a, Type *b) {
	try {
		cast(*(Context *) NULL, a, NULL, b);
		return true;
	} catch (...) {
		return false;
	}
}

llvm::Value* Type::cast(Context &context, Type *otype, llvm::Value *val, Type *dtype) {
	switch (dtype->baseType) {
	case INT:
	case SHORT:
	case BYTE:
	case BOOL:
		if (otype->isInt())
			if (otype->getSize(context) == dtype->getSize(context))
				return val;
			else {
				if (otype->getSize(context) > dtype->getSize(context))
					return val ? context.getBuilder().CreateTrunc(val, dtype->getType(context)) : NULL;
				else {
					if (otype->isUnsigned)
						return val ? context.getBuilder().CreateBitCast(val, dtype->getType(context)) : NULL;
					else
						return val ? context.getBuilder().CreateSExt(val, dtype->getType(context)) : NULL;
				}
			}
		else if (otype->isFloat()) {
			if (dtype->isUnsigned)
				return val ? context.getBuilder().CreateFPToUI(val, dtype->getType(context)) : NULL;
			else
				return val ? context.getBuilder().CreateFPToSI(val, dtype->getType(context)) : NULL;
		}
		break;
	case FLOAT:
	case DOUBLE:
		if (otype->isInt()) {
			if (otype->isUnsigned)
				return val ? context.getBuilder().CreateUIToFP(val, dtype->getType(context)) : NULL;
			else
				return val ? context.getBuilder().CreateSIToFP(val, dtype->getType(context)) : NULL;
		} else if (otype->isFloat())
			return val ? context.getBuilder().CreateFPCast(val, dtype->getType(context)) : NULL;
		break;
	case OBJECT:
		if (otype->isObject()) {
			Class *ocls = otype->getClass();
			Class *dcls = dtype->getClass();
			if (ocls == dcls)
				return val;
			if (dcls->getMangleName()[0] == 'J') {
				if (ocls->getMangleName()[0] == 'J')
					break;
				for (std::list<std::pair<Interface*, size_t> >::iterator it = ocls->implementsType.begin(); it != ocls->implementsType.end(); it++)
					if (it->first == (Interface *) dcls) {
						llvm::SmallVector<llvm::Value*, 1> tmp;
						tmp.push_back(context.getBuilder().getInt32(it->second));
						return val ? context.getBuilder().CreateIntToPtr(
								context.getBuilder().CreateAdd(
										context.getBuilder().CreatePtrToInt(
												val,
												context.getBuilder().getInt32Ty()
										),
										context.getBuilder().getInt32(context.DL->getStructLayout((llvm::StructType *) ocls->getLLVMType())->getElementOffset(it->second)
										)
								),
								llvm::PointerType::get(dcls->getLLVMType(), 0)
						) : NULL;
					}
			} else {
				while (ocls && ocls != dcls)
					ocls = ocls->extendsClass;
				if (ocls)
					return val ? context.getBuilder().CreatePointerCast(
						val,
						dcls->getType()->getType(context)
					) : NULL;
				break;
			}
		}
		break;
	case ARRAY:
		throw NotImplemented("type cast about array");
		break;
	}
	throw IncompatibleType(std::string("can't cast ") + otype->getName() + " to " + dtype->getName());
}

const std::string Type::getMangleName(Context &context) {
	switch (baseType) {
	case INT:
		if (isUnsigned)
			return "I";
		else
			return "i";
	case SHORT:
		if (isUnsigned)
			return "S";
		else
			return "s";
	case BYTE:
		if (isUnsigned)
			return "B";
		else
			return "b";
	case CHAR:
		return "c";
	case FLOAT:
		return "f";
	case DOUBLE:
		return "d";
	case STRING:
		return "a";
	case BOOL:
		return "t";
	case ARRAY: {
		std::string tmp = "A";
		tmp += itos(arrayDim.size());
		for (std::vector<std::pair<Expression*, Expression*> >::const_iterator it = arrayDim.begin(); it != arrayDim.end(); it++)
			if (!it->first->isConstant() || !it->second || !it->second->isConstant())
				tmp += "D";
			else
				tmp += "S" + itos(it->second->loadConstant()._int64 - it->first->loadConstant()._int64 + 1);
		tmp += internal->getMangleName(context);
		return tmp;
	}
	case OBJECT:
		if (getClass(context) == NULL)
			throw SymbolNotFound("Class '" + identifier->getName() + "'");
		return getClass()->getMangleName();
	}
}

const std::string Type::getMangleName() {
	switch (baseType) {
	case INT:
		if (isUnsigned)
			return "I";
		else
			return "i";
	case SHORT:
		if (isUnsigned)
			return "S";
		else
			return "s";
	case BYTE:
		if (isUnsigned)
			return "B";
		else
			return "b";
	case CHAR:
		return "c";
	case FLOAT:
		return "f";
	case DOUBLE:
		return "d";
	case STRING:
		return "a";
	case BOOL:
		return "t";
	case ARRAY: {
		std::string tmp = "A";
		tmp += itos(arrayDim.size());
		for (std::vector<std::pair<Expression*, Expression*> >::const_iterator it = arrayDim.begin(); it != arrayDim.end(); it++)
			if (!it->first->isConstant() || !it->second || !it->second->isConstant())
				tmp += "D";
			else
				tmp += "S" + itos(it->second->loadConstant()._int64 - it->first->loadConstant()._int64 + 1);
		tmp += internal->getMangleName();
		return tmp;
	}
	case OBJECT:
		if (getClass() == NULL)
			throw SymbolNotFound("Class '" + identifier->getName() + "'");
		return getClass()->getMangleName();
	}
}

llvm::Constant* Type::getDefault(Context &context) {
	switch (baseType) {
	case BYTE:
		return context.getBuilder().getInt8(0);
	case SHORT:
		return context.getBuilder().getInt16(0);
	case INT:
		return context.getBuilder().getInt32(0);
	case ARRAY:
		{
			size_t totalSize = 1;
			for (std::vector<std::pair<Expression*, Expression*> >::const_iterator it = arrayDim.begin(); it != arrayDim.end(); it++) {
				if (!it->second)
					throw NotImplemented("dynamic array");
				if (!it->first->isConstant() || !it->second->isConstant())
					throw NotImplemented("dynamic dim expression");
				if (!it->first->getTypeConstant()->isInt() || !it->second->getTypeConstant()->isInt())
					throw InvalidType("dim expression must be integer");
				totalSize *= it->second->loadConstant()._int64 - it->first->loadConstant()._int64 + 1;
			}
			llvm::SmallVector<llvm::Constant*, 16> tmp;
			llvm::Constant *internal_default = internal->getDefault(context);
			for (size_t i = 0; i < totalSize; i++)
				tmp.push_back(internal_default);
			return llvm::ConstantArray::get((llvm::ArrayType *) getType(context), tmp);
		}
		break;
	case STRING:
	case OBJECT:
		return llvm::ConstantPointerNull::get((llvm::PointerType *) getType(context));
	}
	throw NotImplemented(std::string("type '") + getName() + "'");
}

Class* Type::getClass(Context &context) {
	if (!cls)
		*const_cast<Class**>(&cls) = context.findClass(identifier->getName());
	return cls;
}

const std::string Type::getName() {
	switch (baseType) {
	case ARRAY:
		return internal->getName() + "[]";
	case OBJECT:
		if (cls)
			return cls->getFullName();
		else
			return identifier->getName();
	default:
		return BaseTypeNames[baseType];
	}
}
