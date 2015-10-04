%option noyywrap
%{
	#include "jascal.tab.hpp"
	#include <stdio.h>

	FILE *lex_output = NULL;

	#define YY_USER_ACTION { \
		yylloc.first_column = yylloc.last_column + 1; \
		yylloc.last_column = yylloc.first_column + yyleng - 1; \
		yylval = NULL; \
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
	yylval = (ASTNode *) PRIVATE;
	return visibility; }
"public"	{
	if (lex_output)
		fprintf(lex_output, "<PUBLIC>");
	yylval = (ASTNode *) PUBLIC;
	return visibility; }
"protected"	{
	if (lex_output)
		fprintf(lex_output, "<PROTECTED>");
	yylval = (ASTNode *) PROTECTED;
	return visibility; }
"return" {
	if (lex_output)
		fprintf(lex_output, "<RETURN>");
	return T_RETURN; }
\"(\\\"|[^\"])*\"	{
	if (lex_output)
		fprintf(lex_output, "<literal_string %s>", yytext);
	yylval = new LiteralString(yytext);
	return literal_string; }
[ ]+	{
	if (lex_output)
		fprintf(lex_output, "<space \"%s\">", yytext);
	return yylex(); }
\t	{
	if (lex_output)
		fprintf(lex_output, "<space \"\\t\">");
	while (yylloc.last_column % 4)
		yylloc.last_column++;
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
