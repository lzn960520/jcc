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
