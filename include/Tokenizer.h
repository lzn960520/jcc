#ifndef _TOKENIZER_H_
#define _TOKENIZER_H_

#undef yyFlexLexer
#include <FlexLexer.h>
#include <istream>
#include <list>

#include "location.h"

class Token;
class Tokenizer : public yyFlexLexer {
	Location yylloc;
	std::string str;
	int v;
	bool b;
	friend std::list<Token*> tokenize(std::istream &is, const std::string &path);
public:
	Tokenizer(std::istream &is, const std::string &path);
	int yylex();
	~Tokenizer();
};

#endif
