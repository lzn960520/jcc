%option noyywrap
%{
	#include "jascal.tab.hpp"
	#include "Identifier.h"
	#include "LiteralString.h"
	#include "LiteralInt.h"
	#include <stdio.h>

	FILE *lex_output = NULL;

	#define YY_USER_ACTION { \
		yylloc.first_column = yylloc.last_column + 1; \
		yylloc.last_column = yylloc.first_column + yyleng - 1; \
		yylval.token = yylloc; \
	}
%}

%%

\/\/.*	{
	if (lex_output)
		fprintf(lex_output, "<comment>");
	return yylex(); }
\/\*(.|\n)*\*\/	{
	if (lex_output)
		fprintf(lex_output, "<comment>");
	return yylex(); }
"+"	{
	if (lex_output)
		fprintf(lex_output, "<ADD>");
	return T_ADD; }
"-"	{
	if (lex_output)
		fprintf(lex_output, "<SUB>");
	return T_SUB; }
"*"	{
	if (lex_output)
		fprintf(lex_output, "<MUL>");
	return T_MUL; }
"/"	{
	if (lex_output)
		fprintf(lex_output, "<DIV>");
	return T_DIV; }
"begin"	{
	if (lex_output)
		fprintf(lex_output, "<BEGIN>");
	return T_BEGIN; }
"end"	{
	if (lex_output)
		fprintf(lex_output, "<END>");
	return T_END; }
";"	{
	if (lex_output)
		fprintf(lex_output, "<SEMICOLON>");
	return T_SEMICOLON; }
"if"	{
	if (lex_output)
		fprintf(lex_output, "<IF>");
	return T_IF; }
"then"	{
	if (lex_output)
		fprintf(lex_output, "<THEN>");
	return T_THEN; }
"else"	{
	if (lex_output)
		fprintf(lex_output, "<ELSE>");
	return T_ELSE; }
"while"	{
	if (lex_output)
		fprintf(lex_output, "<WHILE>");
	return T_WHILE; }
"do"	{
	if (lex_output)
		fprintf(lex_output, "<DO>");
	return T_DO; }
"("	{
	if (lex_output)
		fprintf(lex_output, "<LEFT_PARENTHESIS>");
	return T_LEFT_PARENTHESIS; }
")"	{
	if (lex_output)
		fprintf(lex_output, "<RIGHT_PARENTHESIS>");
	return T_RIGHT_PARENTHESIS; }
"<" {
	if (lex_output)
		fprintf(lex_output, "<LESS>");
	return T_LT; }
">" {
	if (lex_output)
		fprintf(lex_output, "<GREATER>");
	return T_GT; }
"<=" {
	if (lex_output)
		fprintf(lex_output, "<LESS_OR_EQUAL>");
	return T_LEQ; }
">=" {
	if (lex_output)
		fprintf(lex_output, "<GREATER_OR_EQUAL>");
	return T_GEQ; }
"++" {
	if (lex_output)
		fprintf(lex_output, "<SELF_INC>");
	return T_SELF_INC; }
"--" {
	if (lex_output)
		fprintf(lex_output, "<SELF_DEC>");
	return T_SELF_DEC; }	
"unsigned"	{
	if (lex_output)
		fprintf(lex_output, "<UNSIGNED>");
	return T_UNSIGNED; }
"byte"	{
	if (lex_output)
		fprintf(lex_output, "<BYTE>");
	return T_BYTE; }
"short"	{
	if (lex_output)
		fprintf(lex_output, "<SHORT>");
	return T_SHORT; }
"int"	{
	if (lex_output)
		fprintf(lex_output, "<INT>");
	return T_INT; }
"char"	{
	if (lex_output)
		fprintf(lex_output, "<CHAR>");
	return T_CHAR; }
"float"	{
	if (lex_output)
		fprintf(lex_output, "<FLOAT>");
	return T_FLOAT; }
"double"	{
	if (lex_output)
		fprintf(lex_output, "<DOUBLE>");
	return T_DOUBLE; }
"="	{
	if (lex_output)
		fprintf(lex_output, "<ASSIGN>");
	return T_ASSIGN; }
","	{
	if (lex_output)
		fprintf(lex_output, "<COMMA>");
	return T_COMMA; }
"private"	{
	if (lex_output)
		fprintf(lex_output, "<PRIVATE>");
	return T_PRIVATE; }
"public"	{
	if (lex_output)
		fprintf(lex_output, "<PUBLIC>");
	return T_PUBLIC; }
"protected"	{
	if (lex_output)
		fprintf(lex_output, "<PROTECTED>");
	return T_PROTECTED; }
"return" {
	if (lex_output)
		fprintf(lex_output, "<RETURN>");
	return T_RETURN; }
"repeat" {
	if (lex_output)
		fprintf(lex_output, "<REPEAT>");
	return T_REPEAT; }
"until" {
	if (lex_output)
		fprintf(lex_output, "<UNTIL>");
	return T_UNTIL; }
":" {
	if (lex_output)
		fprintf(lex_output, "<COLON>");
	return T_COLON; }
"function" {
	if (lex_output)
		fprintf(lex_output, "<FUNCTION>");
	return T_FUNCTION; }
"procedure" {
	if (lex_output)
		fprintf(lex_output, "<PROCEDURE>");
	return T_PROCEDURE; }
"[" {
	if (lex_output)
		fprintf(lex_output, "<LEFT_SQUARE>");
	return T_LEFT_SQUARE; }
"]" {
	if (lex_output)
		fprintf(lex_output, "<RIGHT_SQUARE>");
	return T_RIGHT_SQUARE; }
"." {
	if (lex_output)
		fprintf(lex_output, "<DOT>");
	return T_DOT; }
".." {
	if (lex_output)
		fprintf(lex_output, "<DOT_DOT>");
	return T_DOTDOT; }
"string" {
	if (lex_output)
		fprintf(lex_output, "<STRING>");
	return T_STRING; }
"==" {
	if (lex_output)
		fprintf(lex_output, "<EQUAL>");
	return T_EQ; }
"!=" {
	if (lex_output)
		fprintf(lex_output, "<NOT_EQUAL>");
	return T_NEQ; }
"<>" {
	if (lex_output)
		fprintf(lex_output, "<NOT_EQUAL>");
	return T_NEQ; }
"&&" {
	if (lex_output)
		fprintf(lex_output, "<LOGIC_AND>");
	return T_LOG_AND; }
"||" {
	if (lex_output)
		fprintf(lex_output, "<LOGIC_OR>");
	return T_LOG_OR; }
"^^" {
	if (lex_output)
		fprintf(lex_output, "<LOGIC_XOR>");
	return T_LOG_XOR; }
"module" {
	if (lex_output)
		fprintf(lex_output, "<MODULE>");
	return T_MODULE; }
"class" {
	if (lex_output)
		fprintf(lex_output, "<CLASS>");
	return T_CLASS; }
"::" {
	if (lex_output)
		fprintf(lex_output, "<COLON_COLON>");
	return T_NS; }
"interface" {
	if (lex_output)
		fprintf(lex_output, "<INTERFACE>");
	return T_INTERFACE; }
"new" {
	if (lex_output)
		fprintf(lex_output, "<NEW>");
	return T_NEW; }
"var" {
	if (lex_output)
		fprintf(lex_output, "<VAR>");
	return T_VAR; }
"static" {
	if (lex_output)
		fprintf(lex_output, "<STATIC>");
	return T_STATIC; }
"const" {
	if (lex_output)
		fprintf(lex_output, "<CONST>");
	return T_CONST; }
"using" {
	if (lex_output)
		fprintf(lex_output, "<USING>");
	return T_USING; }
\"(\\\"|[^\"])*\"	{
	if (lex_output)
		fprintf(lex_output, "<literal_string %s>", yytext);
	yylval.expression = new LiteralString(yytext);
	yylval.expression->loc = yylloc;
	return T_LITERAL_STRING; }
[ ]+	{
	if (lex_output)
		fprintf(lex_output, "<space \"%s\">", yytext);
	return yylex(); }
\t	{
	if (lex_output)
		fprintf(lex_output, "<space \"\\t\">");
	return yylex(); }
[\n\r]	{
	if (lex_output) {
		fprintf(lex_output, "<space \"");
		switch (*yytext) {
		case '\n':
			fprintf(lex_output, "\\n");
			break;
		case '\r':
			fprintf(lex_output, "\\r");
			break;
		default:
			fprintf(lex_output, "%c", *yytext);
			break;
		}
		fprintf(lex_output, "\">");
		if (*yytext == '\n') {
			fprintf(lex_output, "\n");
			yylloc.first_line++;
			yylloc.last_line++;
			yylloc.first_column = yylloc.last_column = 0;
		}
	}
	return yylex(); }
[0-9]+	{
	yylval.expression = new LiteralInt(atoi(yytext), false);
	yylval.expression->loc = yylloc;
	if (lex_output)
		fprintf(lex_output, "<literal_int %s>", yytext);
	return T_LITERAL_INT; }
[0-9]+(u|U) {
	yylval.expression = new LiteralInt(atoi(yytext), true);
	yylval.expression->loc = yylloc;
	if (lex_output)
		fprintf(lex_output, "<literal_int %s>", yytext);
	return T_LITERAL_INT; }
[a-zA-Z_][a-zA-Z_0-9]*	{
	yylval.identifier = new Identifier(yytext);
	yylval.identifier->loc = yylloc;
	if (lex_output)
		fprintf(lex_output, "<identifier %s>", yytext);
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
