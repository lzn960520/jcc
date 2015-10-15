#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <string>
#include "yyvaltypes.h"

class ArgumentException {
protected:
	std::string msg;
public:
	ArgumentException(const std::string &msg);
	virtual std::string message() const;
	virtual ~ArgumentException();
};

class CompileException {
protected:
	std::string msg;
public:
	CompileException(const std::string &msg);
	CompileException(const char *msg);
	virtual std::string message() const;
	virtual ~CompileException();
};

class InvalidString : public CompileException {
public:
	InvalidString(const std::string &msg);
	InvalidString(const char *msg);
	std::string message() const;
};

class InvalidType : public CompileException {
public:
	InvalidType(const char *msg);
	InvalidType(const std::string &msg);
	std::string message() const;
};

class FunctionNotFound : public CompileException {
public:
	FunctionNotFound(const char *msg);
	FunctionNotFound(const std::string &msg);
	std::string message() const;
};

class SymbolNotFound : public CompileException {
	YYLTYPE loc;
	bool has_loc;
public:
	SymbolNotFound(const char *msg);
	SymbolNotFound(const std::string &msg);
	SymbolNotFound(const std::string &msg, YYLTYPE loc);
	std::string message() const;
};

class NotImplemented : public CompileException {
public:
	NotImplemented(const char *msg);
	NotImplemented(const std::string &msg);
	std::string message() const;
};

class Redefination : public CompileException {
public:
	Redefination(const char *msg);
	Redefination(const std::string &msg);
	std::string message() const;
};

class NoReturn : public CompileException {
public:
	NoReturn(const char *msg);
	NoReturn(const std::string &msg);
	std::string message() const;
};

class NotAssignable : public CompileException {
public:
	NotAssignable(const char *msg);
	NotAssignable(const std::string &msg);
	std::string message() const;
};

class NotArray : public CompileException {
public:
	NotArray(const char *msg);
	NotArray(const std::string &msg);
	std::string message() const;
};

class DimMismatch : public CompileException {
public:
	DimMismatch(const char *msg);
	DimMismatch(const std::string &msg);
	std::string message() const;
};

class IncompatibleType : public CompileException {
public:
	IncompatibleType(const std::string &type1, const std::string &type2);
	std::string message() const;
};

class NotConstant : public CompileException {
public:
	NotConstant(const char *msg);
	NotConstant(const std::string &msg);
	std::string message() const;
};

#endif
