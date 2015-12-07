#include "Tokenizer.h"

Tokenizer::Tokenizer(std::istream &is, const std::string &path) {
	yyin = &is;
	yylloc.begin.filename = path;
	yylloc.begin.column = 0;
	yylloc.begin.line = 1;
	yylloc.end = yylloc.begin;
}

Tokenizer::~Tokenizer() {
}
