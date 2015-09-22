%code requires {
	#include "yyvaltypes.h"
	#define YYSTYPE ASTNode*
}

%code top {
	#define YYERROR_VERBOSE 1
}

%initial-action {
	yylloc.last_column = 0;
}

%code {
	#include <cstdio>
	#include <fstream>
	#include <cstring>

	int yylex (void);
	void yyerror (const char *);

	// global
	ASTNode *root = NULL;

	// from main
	extern const char *input_filename;
}

%locations
	
%token literal_int
%token literal_string
%token identifier
%token T_MODULE
%token T_BEGIN
%token T_END
%token T_LEFT_CURLY
%token T_RIGHT_CURLY
%token T_SEMICOLON
%token T_IF
%token T_LEFT_PARENTHESIS
%token T_RIGHT_PARENTHESIS
%token T_THEN
%token T_ELSE

%left T_ADD T_SUB
%left T_MUL T_DIV
%left T_MOD
%left T_PWR

%%

compile_unit:
	statement {
		root = $$; }

expression:
	expression T_ADD expression {
		$$ = new Expression($1, Expression::ADD, $3); }
	| expression T_SUB expression {
		$$ = new Expression($1, Expression::SUB, $3); }
	| expression T_MUL expression {
		$$ = new Expression($1, Expression::MUL, $3); }
	| expression T_DIV expression {
		$$ = new Expression($1, Expression::DIV, $3); }
	| literal
	| identifier

literal:
	literal_int
	| literal_string

statement:
	T_SEMICOLON
	| expression T_SEMICOLON {
		$$ = $1; }
	| if_statement
	| T_BEGIN statements T_END {
		$$ = $2; }
	| T_BEGIN T_END {
		$$ = new Statements(NULL); }

statements:
	statement {
		$$ = new Statements($1); }
	| statements statement {
		((Statements*) $1)->push_back($2);
		$$ = $1; }
if_statement:
	T_IF T_LEFT_PARENTHESIS expression T_RIGHT_PARENTHESIS T_THEN statement {
		$$ = new IfStatement($3, $6); }
	| T_IF T_LEFT_PARENTHESIS expression T_RIGHT_PARENTHESIS T_THEN statement T_ELSE statement {
		$$ = new IfStatement($3, $6, $8); }
%%

void yyerror(const char *s) {
	if (input_filename) {
		std::ifstream ifs(input_filename);
		std::string line;
		for (int i = 0; i < yylloc.first_line; i++)
			getline(ifs, line);
		int len = printf("%s:%-5d", input_filename, yylloc.first_line);
		printf("%s\n", line.c_str());
		for (int i = -len; i < yylloc.first_column - 1; i++)
			printf(" ");
		for (int i = yylloc.first_column; i <= yylloc.last_column; i++)
			printf("^");
		printf("\n");
	}
	printf("error: %s at %d:%d to %d:%d\n", s, yylloc.first_line, yylloc.first_column, yylloc.last_line, yylloc.last_column);
}
