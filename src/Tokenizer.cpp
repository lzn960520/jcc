#include "Tokenizer.h"

Tokenizer::Tokenizer(std::istream &is) {
	yyin = &is;
	yylloc.begin.column = 0;
	yylloc.begin.line = 1;
	yylloc.end = yylloc.begin;
}

Tokenizer::~Tokenizer() {
}
