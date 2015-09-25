#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <string>

class CompileException {
public:
	virtual std::string message() = 0;
	virtual ~CompileException() {};
};

class InvalidType : public CompileException {
	std::string msg;
public:
	InvalidType() {};
	InvalidType(const char *msg) : msg(msg) {};
	std::string message() { return "Invalid type: " + msg; }
};

class FunctionNotFound : public CompileException {
	std::string msg;
public:
	FunctionNotFound() {};
	FunctionNotFound(const char *msg) : msg(msg) {};
	FunctionNotFound(std::string msg) : msg(msg) {};
	std::string message() { return "Function not found: " + msg; }
};

#endif
