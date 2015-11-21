#ifndef _TOKEN_H_
#define _TOKEN_H_

#include <ostream>
#include <list>

#include "location.h"

class Parser;
class Token {
	int type;
	Location yylloc;
	std::string str;
	int v;
	bool isUnsigned;
	friend class Parser;
public:
	Token(int type, const Location &yylloc) : type(type), yylloc(yylloc) {}
	Token(int type, const Location &yylloc, const std::string &str) : type(type), yylloc(yylloc), str(str) {}
	Token(int type, const Location &yylloc, const std::string &str, int v, bool isUnsigned) : type(type), yylloc(yylloc), str(str), v(v), isUnsigned(isUnsigned) {}
	inline const Location& getLocation() { return yylloc; }
	friend std::ostream& operator<< (std::ostream &os, const Token &token);
};
std::ostream& operator<< (std::ostream &os, const Token &token);
std::ostream& operator<< (std::ostream &os, const std::list<Token*> &tokens);
extern const char * const * const tokenNames;

#endif
