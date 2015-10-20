#include "exception.h"
#include "util.h"

using namespace std;

ArgumentException::ArgumentException(const string &msg) : msg(msg) {}
string ArgumentException::message() const {
	return msg;
}
ArgumentException::~ArgumentException() {}

CompileException::CompileException(const char *msg) : msg(msg) {}
CompileException::CompileException(const string &msg) : msg(msg) {}
string CompileException::message() const {
	return msg;
}
CompileException::~CompileException() {}

InvalidString::InvalidString(const char *msg) : CompileException(msg) {}
InvalidString::InvalidString(const string &msg) : CompileException(msg) {}
string InvalidString::message() const {
	return "Invalid string: " + msg;
}

InvalidType::InvalidType(const char *msg) : CompileException(msg) {}
InvalidType::InvalidType(const string &msg) : CompileException(msg) {}
string InvalidType::message() const {
	return "Invalid type: " + msg;
}

FunctionNotFound::FunctionNotFound(const char *msg) : CompileException(msg) {}
FunctionNotFound::FunctionNotFound(const string &msg) : CompileException(msg) {}
string FunctionNotFound::message() const {
	return "Function not found: " + msg;
}

SymbolNotFound::SymbolNotFound(const char *msg) : CompileException(msg), has_loc(false) {}
SymbolNotFound::SymbolNotFound(const string &msg) : CompileException(msg), has_loc(false) {}
SymbolNotFound::SymbolNotFound(const string &msg, YYLTYPE loc) : CompileException(msg), loc(loc), has_loc(true) {}
string SymbolNotFound::message() const {
	if (has_loc)
		return "Symbol not found: " + msg + " (at " + itos(loc.first_line) + ")";
	else
		return "Symbol not found: " + msg;
}

NotImplemented::NotImplemented(const char *msg) : CompileException(msg) {}
NotImplemented::NotImplemented(const string &msg) : CompileException(msg) {}
string NotImplemented::message() const {
	return "Not implemented: " + msg;
}

Redefination::Redefination(const char *msg) : CompileException(msg) {}
Redefination::Redefination(const string &msg) : CompileException(msg) {}
string Redefination::message() const {
	return "Variable " + msg + " redefined";
}

NoReturn::NoReturn(const char *msg) : CompileException(msg) {}
NoReturn::NoReturn(const string &msg) : CompileException(msg) {}
string NoReturn::message() const {
	return "Not all branches of " + msg + " have return";
}

NotAssignable::NotAssignable(const char *msg) : CompileException(msg) {}
NotAssignable::NotAssignable(const string &msg) : CompileException(msg) {}
string NotAssignable::message() const {
	return "Can't assign value to " + msg;
}

NotArray::NotArray(const char *msg) : CompileException(msg) {}
NotArray::NotArray(const string &msg) : CompileException(msg) {}
string NotArray::message() const {
	return msg + " isn't an array";
}

DimMismatch::DimMismatch(const char *msg) : CompileException(msg) {}
DimMismatch::DimMismatch(const string &msg) : CompileException(msg) {}
string DimMismatch::message() const {
	return "Dim mismatch between array " + msg + " and accessor";
}

IncompatibleType::IncompatibleType(const string &type1, const string &type2) : CompileException("Incompatible type " + type1 + " and " + type2) {}
string IncompatibleType::message() const {
	return msg;
}

NotConstant::NotConstant(const char *msg) : CompileException(msg) {}
NotConstant::NotConstant(const string &msg) : CompileException(msg) {}
string NotConstant::message() const {
	return msg + " is not constant";
}
