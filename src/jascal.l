%option noyywrap
%option c++
%option yyclass="Tokenizer"

%{
	#include "jascal.tab.hpp"
	#include "Identifier.h"
	#include "LiteralString.h"
	#include "LiteralInt.h"
	#include "Tokenizer.h"

	#define YY_USER_ACTION { \
		yylloc.begin = yylloc.end; \
		yylloc.begin.column++; \
		yylloc.end.column += strlen(YYText()); }
	static inline unsigned int strcnt(const char *str, char c) {
		unsigned int cnt = 0;
		while (*str)
			cnt += (*str++ == c);
		return cnt;
	}
%}

%%

^#lib {
	return T_LIB; }
^#entry {
	return T_ENTRY; }
\/\/.*	{
	return T_COMMENT; }
\/\*(.|\n)*\*\/	{
	yylloc.end.line += strcnt(yytext, '\n');
	yylloc.end.column -= (strrchr(yytext, '\n') - yytext);
	return T_COMMENT; }
"+"	{
	return T_ADD; }
"-"	{
	return T_SUB; }
"*"	{
	return T_MUL; }
"/"	{
	return T_DIV; }
"begin"	{
	return T_BEGIN; }
"end"	{
	return T_END; }
";"	{
	return T_SEMICOLON; }
"if"	{
	return T_IF; }
"then"	{
	return T_THEN; }
"else"	{
	return T_ELSE; }
"while"	{
	return T_WHILE; }
"do"	{
	return T_DO; }
"("	{
	return T_LEFT_PARENTHESIS; }
")"	{
	return T_RIGHT_PARENTHESIS; }
"<" {
	return T_LT; }
">" {
	return T_GT; }
"<=" {
	return T_LEQ; }
">=" {
	return T_GEQ; }
"++" {
	return T_SELF_INC; }
"--" {
	return T_SELF_DEC; }	
"unsigned"	{
	return T_UNSIGNED; }
"byte"	{
	return T_BYTE; }
"short"	{
	return T_SHORT; }
"int"	{
	return T_INT; }
"char"	{
	return T_CHAR; }
"float"	{
	return T_FLOAT; }
"double"	{
	return T_DOUBLE; }
"="	{
	return T_ASSIGN; }
","	{
	return T_COMMA; }
"private"	{
	return T_PRIVATE; }
"public"	{
	return T_PUBLIC; }
"protected"	{
	return T_PROTECTED; }
"return" {
	return T_RETURN; }
"repeat" {
	return T_REPEAT; }
"until" {
	return T_UNTIL; }
":" {
	return T_COLON; }
"function" {
	return T_FUNCTION; }
"procedure" {
	return T_PROCEDURE; }
"[" {
	return T_LEFT_SQUARE; }
"]" {
	return T_RIGHT_SQUARE; }
"." {
	return T_DOT; }
".." {
	return T_DOTDOT; }
"==" {
	return T_EQ; }
"!=" {
	return T_NEQ; }
"<>" {
	return T_LTGT; }
"&&" {
	return T_LOG_AND; }
"||" {
	return T_LOG_OR; }
"^^" {
	return T_LOG_XOR; }
"module" {
	return T_MODULE; }
"class" {
	return T_CLASS; }
"::" {
	return T_NS; }
"interface" {
	return T_INTERFACE; }
"new" {
	return T_NEW; }
"var" {
	return T_VAR; }
"static" {
	return T_STATIC; }
"const" {
	return T_CONST; }
"using" {
	return T_USING; }
"extends" {
	return T_EXTENDS; }
"implements" {
	return T_IMPLEMENTS; }
"[[" {
	return T_LEFT_LEFT_SQUARE; }
"]]" {
	return T_RIGHT_RIGHT_SQUARE; }
"%" {
	return T_MOD; }
"**" {
	return T_PWR; }
"|=" {
	return T_BIT_OR_ASSIGN; }
"&=" {
	return T_BIT_AND_ASSIGN; }
"^=" {
	return T_BIT_XOR_ASSIGN; }
"||=" {
	return T_LOG_OR_ASSIGN; }
"&&=" {
	return T_LOG_AND_ASSIGN; }
"^^=" {
	return T_LOG_XOR_ASSIGN; }
"!" {
	return T_LOG_NOT; }
"+=" {
	return T_ADD_ASSIGN; }
"-=" {
	return T_SUB_ASSIGN; }
"*=" {
	return T_MUL_ASSIGN; }
"/=" {
	return T_DIV_ASSIGN; }
"%=" {
	return T_MOD_ASSIGN; }
"**=" {
	return T_PWR_ASSIGN; }
"<<" {
	return T_LSH; }
">>" {
	return T_RSH; }
"<<=" {
	return T_LSH_ASSIGN; }
">>=" {
	return T_RSH_ASSIGN; }
"|" {
	return T_BIT_OR; }
"&" {
	return T_BIT_AND; }
"^" {
	return T_BIT_XOR; }
"~" {
	return T_BIT_NOT; }
"bool" {
	return T_BOOL; }
"true" {
	return T_TRUE; }
"false" {
	return T_FALSE; }
\"(\\\"|[^\"])*\"	{
	str = yytext;
	return T_LITERAL_STRING; }
[ ]+	{
	return yylex(); }
\t	{
	return T_TAB; }
[\n\r]	{
	if (*yytext == '\n') {
		yylloc.begin.line++;
		yylloc.begin.column = 0;
		yylloc.end = yylloc.begin;
	}
	if (*yytext == '\n')
		return T_NEWLINE;
	else
		return yylex(); }
[0-9]+	{
	str = yytext;
	v = atoi(yytext);
	b = false;
	return T_LITERAL_INT; }
[0-9]+(u|U) {
	str = yytext;
	v = atoi(yytext);
	b = true;
	return T_LITERAL_INT; }
[a-zA-Z_][a-zA-Z_0-9]*	{
	str = yytext;
	return T_IDENTIFIER; }
. {
	fprintf(stderr, "Unknown token %s\n", yytext);
	return yylex(); }

%%
