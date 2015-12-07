#ifndef _COMPILE_H_
#define _COMPILE_H_

#include <string>
#include <ostream>
#include <istream>
#include <list>

class CompileFile;
class Token;
class Context;

std::list<Token*> tokenize(std::istream &is, const std::string &path = "");
CompileFile* parse(const std::list<Token*> &tokens);
void genSym(CompileFile *root, std::ostream &os);
void compile(CompileFile *root, Context &context);

#endif
