%option noyywrap
%option yylineno
%{
	#include "jascal.tab.h"
	#include <stdio.h>

	FILE *lex_output = NULL;

	#define YY_USER_ACTION { \
		yylloc.first_line = yylloc.last_line = yylineno; \
		yylloc.first_column = yylloc.last_column + 1; \
		yylloc.last_column = yylloc.first_column + yyleng - 1; \
	}
%}

%%

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
"("	{
	if (lex_output)
		fprintf(lex_output, "<LEFT_PARENTHESIS>");
	return T_LEFT_PARENTHESIS; }
")"	{
	if (lex_output)
		fprintf(lex_output, "<RIGHT_PARENTHESIS>");
	return T_RIGHT_PARENTHESIS; }
\"(\\\"|[^\"])*\"	{
	if (lex_output)
		fprintf(lex_output, "<literal_string %s>", yytext);
	yylval = new LiteralString(yytext);
	return literal_string; }
[ \n\t\r]+	{
	if (lex_output) {
		bool need_newline = false;
		fprintf(lex_output, "<space \"");
		for (char *p = yytext; *p; p++) {
			switch (*p) {
			case '\n':
				fprintf(lex_output, "\\n");
				need_newline = true;
				break;
			case '\t':
				fprintf(lex_output, "\\t");
				break;
			case '\r':
				fprintf(lex_output, "\\r");
				break;
			default:
				fprintf(lex_output, "%c", *p);
				break;
			}
		}
		fprintf(lex_output, "\">");
		if (need_newline)
			fprintf(lex_output, "\n");
	}
	return yylex(); }
[0-9]+	{
	yylval = new LiteralInt(atoi(yytext));
	if (lex_output)
		fprintf(lex_output, "<literal_int %s>", yytext);
	return literal_int; }
[a-zA-Z_][a-zA-Z_0-9]*	{
	yylval = new Identifier(yytext);
	if (lex_output)
		fprintf(lex_output, "<identifier %s>", yytext);
	return identifier; }

%%

void lex_only() {
	int token;
	for (;;) {
		token = yylex();
		if (token == 0)
			break;
	}
}
