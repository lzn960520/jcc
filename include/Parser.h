#ifndef _PARSER_H_
#define _PARSER_H_

#include <list>

#include "jascal.tab.hpp"

class Token;
class CompileFile;
class Parser {
	std::list<Token*> list;
	std::list<Token*>::iterator it;
	std::string dirpath;
public:
	Parser(const std::string &dirpath, const std::list<Token*> &list) : list(list), dirpath(dirpath) {}
	Parser(const std::string &dirpath, std::list<Token*> &&list) : list(list), dirpath(dirpath) {}
	inline const std::string& getDirPath() { return dirpath; }
	int yylex(YYSTYPE *yylval, YYLTYPE *yylloc);
	CompileFile *parse();
};

#endif
