#include "compile.h"
#include "Tokenizer.h"
#include "CompileFile.h"
#include "Parser.h"
#include "Token.h"
#include "util.h"
#include "Identifier.h"

std::list<Token*> tokenize(std::istream &is) {
	Tokenizer tokenizer(is);
	std::list<Token*> ans;
	int type;
	while ((type = tokenizer.yylex()) != 0)
		switch (type) {
		case T_IDENTIFIER:
			ans.push_back(new Token(T_IDENTIFIER, tokenizer.yylloc, tokenizer.str));
			break;
		case T_LITERAL_STRING:
			ans.push_back(new Token(T_LITERAL_STRING, tokenizer.yylloc, tokenizer.str));
			break;
		case T_LITERAL_INT:
			ans.push_back(new Token(T_LITERAL_INT, tokenizer.yylloc, tokenizer.v, tokenizer.b));
			break;
		default:
			ans.push_back(new Token(type, tokenizer.yylloc));
			break;
		}
	return ans;
}

CompileFile* parse(const std::list<Token*> tokens) {
	if (tokens.empty())
		return NULL;
	if (!tokens.front()->getLocation().begin.filename.empty()) {
		Parser parser(getDir(tokens.front()->getLocation().begin.filename), tokens);
		return parser.parse();
	} else {
		Parser parser("", tokens);
		return parser.parse();
	}
}

void genSym(CompileFile *root, std::ostream &os) {
	root->writeJsymFile(os);
}

void compile(CompileFile *root, Context &context) {
	root->genStruct(context);
	root->gen(context);
}
