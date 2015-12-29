#ifndef _TYPE_H_
#define _TYPE_H_

#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <vector>

#include "StructNode.h"

class ArrayDefinator;
class Identifier;
class Class;
class Context;
class Expression;
class Type : public StructNode {
public:
	const enum BaseType {
		BOOL,
		BYTE,
		SHORT,
		INT,
		FLOAT,
		DOUBLE,
		CHAR,
		ARRAY,
		STRING,
		OBJECT
	} baseType;
	static const char *BaseTypeNames[];
	static Type Bool;
	static Type _String;
	static Type String;
	static Type Int32;
	static Type UInt32;
	static Type Double;
	static Type Float;
private:
	Type(Identifier *identifier, int);
	Class * const cls;
public:
	const bool isUnsigned;
	const std::vector<std::pair<Expression*, Expression*> > arrayDim;
	Type * const internal;
	Identifier * const identifier;
	Type(BaseType baseType, bool isUnsigned = false);
	Type(Type *internal, ArrayDefinator *definator);
	Type(Type *internal, const std::vector<std::pair<int, int> > &dims);
	Type(Class *cls);
	Type(Identifier *identifier);
	~Type();
	Json::Value json() override;
	llvm::Type *getType(Context &context);
	size_t getSize(Context &context);
	bool isInt() { return baseType == BYTE || baseType == SHORT || baseType == INT || baseType == BOOL; }
	bool isBool() { return baseType == BOOL; }
	bool isFloat() { return baseType == FLOAT || baseType == DOUBLE; }
	bool isNumber() { return isInt() || isFloat(); }
	bool isChar() { return baseType == CHAR; }
	bool isArray() { return baseType == ARRAY; }
	bool isString() { return baseType == STRING; }
	bool isObject() { return baseType == OBJECT; }
	Class* getClass() const { return cls; }
	Class* getClass(Context &context);
	static Type* higherType(Type *a, Type *b);
	const std::string getName();
	static llvm::Value* cast(Context &context, Type *otype, llvm::Value *val, Type *dtype);
	void writeJsymFile(std::ostream &os) override;
	const std::string getMangleName(Context &context) const;
	const std::string getMangleName() const;
	void gen(Context &context) override {};
	void genStruct(Context &context) override {};
	llvm::Constant* getDefault(Context &context);
	static bool isA(Type *a, Type *b);
	bool equals (const Type &type, Context &context) const;
	Type* clone() const;
	static int distance(Type *from, Type *to, Context &context);
};

#endif
