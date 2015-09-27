#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <string>

class CompileException {
protected:
	std::string msg;
public:
	CompileException();
	CompileException(const std::string &msg);
	CompileException(const char *msg);
	virtual std::string message() const;
	virtual ~CompileException();
};

class InvalidString : public CompileException {
public:
	InvalidString();
	InvalidString(const std::string &msg);
	InvalidString(const char *msg);
	std::string message() const;
};

class InvalidType : public CompileException {
public:
	InvalidType();
	InvalidType(const char *msg);
	InvalidType(const std::string &msg);
	std::string message() const;
};

class FunctionNotFound : public CompileException {
public:
	FunctionNotFound();
	FunctionNotFound(const char *msg);
	FunctionNotFound(const std::string &msg);
	std::string message() const;
};

#endif
