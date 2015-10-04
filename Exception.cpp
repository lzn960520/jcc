#include "exception.h"
#include <sstream>

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

static std::string itos(int i) {
	std::ostringstream os;
	os << i;
	return os.str();
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
