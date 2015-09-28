#include "exception.h"

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

NotImplemented::NotImplemented(const char *msg) : CompileException(msg) {}
NotImplemented::NotImplemented(const string &msg) : CompileException(msg) {}
string NotImplemented::message() const {
	return "Not implemented: " + msg;
}
