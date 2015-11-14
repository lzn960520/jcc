#include "Parser.h"

CompileFile* Parser::parse() {
	CompileFile *ans = NULL;
	it = list.begin();
	yyparse(*this, ans);
	return ans;
}
