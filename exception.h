#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <string>

class InvalidType {
	std::string msg;
public:
	InvalidType() {};
	InvalidType(const char *msg) : msg(msg) {};
	std::string message() { return "Invalid type: " + msg; }
};

#endif
