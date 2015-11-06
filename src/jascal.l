%option noyywrap
%{
	#include "jascal.tab.hpp"
	#include "Identifier.h"
	#include "LiteralString.h"
	#include "LiteralInt.h"
	#include <stdio.h>

	typedef void (*lex_output_func)(char *fmt, ...);
	lex_output_func lex_output = NULL;

	#define YY_USER_ACTION { \
		yylloc.first_column = yylloc.last_column + 1; \
		yylloc.last_column = yylloc.first_column + yyleng - 1; \
		yylval.token = yylloc; \
	}

	#define TOKEN_OUTPUT(fmt, arg...) lex_output(fmt, ##arg)
%}

%%

\/\/.*	{
	TOKEN_OUTPUT("<comment>");
	return yylex(); }
\/\*(.|\n)*\*\/	{
	TOKEN_OUTPUT("<comment>");
	return yylex(); }
"+"	{
	TOKEN_OUTPUT("<ADD>");
	return T_ADD; }
"-"	{
	TOKEN_OUTPUT("<SUB>");
	return T_SUB; }
"*"	{
	TOKEN_OUTPUT("<MUL>");
	return T_MUL; }
"/"	{
	TOKEN_OUTPUT("<DIV>");
	return T_DIV; }
"begin"	{
	TOKEN_OUTPUT("<BEGIN>");
	return T_BEGIN; }
"end"	{
	TOKEN_OUTPUT("<END>");
	return T_END; }
";"	{
	TOKEN_OUTPUT("<SEMICOLON>");
	return T_SEMICOLON; }
"if"	{
	TOKEN_OUTPUT("<IF>");
	return T_IF; }
"then"	{
	TOKEN_OUTPUT("<THEN>");
	return T_THEN; }
"else"	{
	TOKEN_OUTPUT("<ELSE>");
	return T_ELSE; }
"while"	{
	TOKEN_OUTPUT("<WHILE>");
	return T_WHILE; }
"do"	{
	TOKEN_OUTPUT("<DO>");
	return T_DO; }
"("	{
	TOKEN_OUTPUT("<LEFT_PARENTHESIS>");
	return T_LEFT_PARENTHESIS; }
")"	{
	TOKEN_OUTPUT("<RIGHT_PARENTHESIS>");
	return T_RIGHT_PARENTHESIS; }
"<" {
	TOKEN_OUTPUT("<LESS>");
	return T_LT; }
">" {
	TOKEN_OUTPUT("<GREATER>");
	return T_GT; }
"<=" {
	TOKEN_OUTPUT("<LESS_OR_EQUAL>");
	return T_LEQ; }
">=" {
	TOKEN_OUTPUT("<GREATER_OR_EQUAL>");
	return T_GEQ; }
"++" {
	TOKEN_OUTPUT("<SELF_INC>");
	return T_SELF_INC; }
"--" {
	TOKEN_OUTPUT("<SELF_DEC>");
	return T_SELF_DEC; }	
"unsigned"	{
	TOKEN_OUTPUT("<UNSIGNED>");
	return T_UNSIGNED; }
"byte"	{
	TOKEN_OUTPUT("<BYTE>");
	return T_BYTE; }
"short"	{
	TOKEN_OUTPUT("<SHORT>");
	return T_SHORT; }
"int"	{
	TOKEN_OUTPUT("<INT>");
	return T_INT; }
"char"	{
	TOKEN_OUTPUT("<CHAR>");
	return T_CHAR; }
"float"	{
	TOKEN_OUTPUT("<FLOAT>");
	return T_FLOAT; }
"double"	{
	TOKEN_OUTPUT("<DOUBLE>");
	return T_DOUBLE; }
"="	{
	TOKEN_OUTPUT("<ASSIGN>");
	return T_ASSIGN; }
","	{
	TOKEN_OUTPUT("<COMMA>");
	return T_COMMA; }
"private"	{
	TOKEN_OUTPUT("<PRIVATE>");
	return T_PRIVATE; }
"public"	{
	TOKEN_OUTPUT("<PUBLIC>");
	return T_PUBLIC; }
"protected"	{
	TOKEN_OUTPUT("<PROTECTED>");
	return T_PROTECTED; }
"return" {
	TOKEN_OUTPUT("<RETURN>");
	return T_RETURN; }
"repeat" {
	TOKEN_OUTPUT("<REPEAT>");
	return T_REPEAT; }
"until" {
	TOKEN_OUTPUT("<UNTIL>");
	return T_UNTIL; }
":" {
	TOKEN_OUTPUT("<COLON>");
	return T_COLON; }
"function" {
	TOKEN_OUTPUT("<FUNCTION>");
	return T_FUNCTION; }
"procedure" {
	TOKEN_OUTPUT("<PROCEDURE>");
	return T_PROCEDURE; }
"[" {
	TOKEN_OUTPUT("<LEFT_SQUARE>");
	return T_LEFT_SQUARE; }
"]" {
	TOKEN_OUTPUT("<RIGHT_SQUARE>");
	return T_RIGHT_SQUARE; }
"." {
	TOKEN_OUTPUT("<DOT>");
	return T_DOT; }
".." {
	TOKEN_OUTPUT("<DOT_DOT>");
	return T_DOTDOT; }
"string" {
	TOKEN_OUTPUT("<STRING>");
	return T_STRING; }
"==" {
	TOKEN_OUTPUT("<EQUAL>");
	return T_EQ; }
"!=" {
	TOKEN_OUTPUT("<NOT_EQUAL>");
	return T_NEQ; }
"<>" {
	TOKEN_OUTPUT("<NOT_EQUAL>");
	return T_NEQ; }
"&&" {
	TOKEN_OUTPUT("<LOGIC_AND>");
	return T_LOG_AND; }
"||" {
	TOKEN_OUTPUT("<LOGIC_OR>");
	return T_LOG_OR; }
"^^" {
	TOKEN_OUTPUT("<LOGIC_XOR>");
	return T_LOG_XOR; }
"module" {
	TOKEN_OUTPUT("<MODULE>");
	return T_MODULE; }
"class" {
	TOKEN_OUTPUT("<CLASS>");
	return T_CLASS; }
"::" {
	TOKEN_OUTPUT("<COLON_COLON>");
	return T_NS; }
"interface" {
	TOKEN_OUTPUT("<INTERFACE>");
	return T_INTERFACE; }
"new" {
	TOKEN_OUTPUT("<NEW>");
	return T_NEW; }
"var" {
	TOKEN_OUTPUT("<VAR>");
	return T_VAR; }
"static" {
	TOKEN_OUTPUT("<STATIC>");
	return T_STATIC; }
"const" {
	TOKEN_OUTPUT("<CONST>");
	return T_CONST; }
"using" {
	TOKEN_OUTPUT("<USING>");
	return T_USING; }
\"(\\\"|[^\"])*\"	{
	TOKEN_OUTPUT("<literal_string %s>", yytext);
	yylval.expression = new LiteralString(yytext);
	yylval.expression->loc = yylloc;
	return T_LITERAL_STRING; }
[ ]+	{
	return yylex(); }
\t	{
	TOKEN_OUTPUT("\t");
	return yylex(); }
[\n\r]	{
	if (*yytext == '\n') {
		yylloc.first_line++;
		yylloc.last_line++;
		yylloc.first_column = yylloc.last_column = 0;
	}
	if (*yytext == '\n')
	TOKEN_OUTPUT("\n");
	return yylex(); }
[0-9]+	{
	yylval.expression = new LiteralInt(atoi(yytext), false);
	yylval.expression->loc = yylloc;
	TOKEN_OUTPUT("<literal_int %s>", yytext);
	return T_LITERAL_INT; }
[0-9]+(u|U) {
	yylval.expression = new LiteralInt(atoi(yytext), true);
	yylval.expression->loc = yylloc;
	TOKEN_OUTPUT("<literal_int %s>", yytext);
	return T_LITERAL_INT; }
[a-zA-Z_][a-zA-Z_0-9]*	{
	yylval.identifier = new Identifier(yytext);
	yylval.identifier->loc = yylloc;
	TOKEN_OUTPUT("<identifier %s>", yytext);
	return T_IDENTIFIER; }
. {
	fprintf(stderr, "Unknown token %s\n", yytext);
	return yylex(); }

%%

void lex_only() {
	int token;
	for (;;) {
		token = yylex();
		if (token == 0)
			break;
	}
}
