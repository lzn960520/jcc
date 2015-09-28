#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <string>

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

class NotImplemented : public CompileException {
public:
	NotImplemented(const char *msg);
	NotImplemented(const std::string &msg);
	std::string message() const;
};
#endif
