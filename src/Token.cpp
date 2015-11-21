#include "Token.h"
#include "Parser.h"
#include "jascal.tab.hpp"
#include "Identifier.h"
#include "LiteralString.h"
#include "LiteralInt.h"
#include <iostream>

int Parser::yylex(YYSTYPE *yylval, YYLTYPE *yylloc) {
	if (it == list.end())
		return 0;
	*yylloc = (*it)->yylloc;
	switch ((*it)->type) {
	case T_IDENTIFIER:
		yylval->identifier = new Identifier((*it)->str.c_str());
		yylval->identifier->loc = (*it)->yylloc;
		break;
	case T_LITERAL_STRING:
		yylval->literal_string = new LiteralString((*it)->str.c_str());
		yylval->literal_string->loc = (*it)->yylloc;
		break;
	case T_LITERAL_INT:
		yylval->expression = new LiteralInt((*it)->v, (*it)->isUnsigned);
		yylval->expression->loc = (*it)->yylloc;
		break;
	case T_NEWLINE:
	case T_TAB:
	case T_COMMENT:
		it++;
		return yylex(yylval, yylloc);
	}
	return (*it++)->type;
}

std::ostream& operator<< (std::ostream &os, const Token &token) {
	switch (token.type) {
	case T_NEWLINE:
		os << '\n';
		break;
	case T_TAB:
		os << '\t';
		break;
	case T_IDENTIFIER:
		os << "<identifier " << token.str << ">";
		break;
	case T_LITERAL_INT:
		os << "<literal_int " << token.str << ">";
		break;
	case T_LITERAL_STRING:
		os << "<literal_string " << token.str << ">";
		break;
	default:
		os << "<";
		for (const char *p = tokenNames[token.type - 255] + 1; *(p + 1); p++)
			os << *p;
		os << ">";
	}
	return os;
}

std::ostream& operator<< (std::ostream &os, const std::list<Token*> &tokens) {
	for (std::list<Token*>::const_iterator it = tokens.begin(); it != tokens.end(); it++)
		os << **it;
	return os;
}
