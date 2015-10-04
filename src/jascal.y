%code requires {
	#include "yyvaltypes.h"
	
	#include "LiteralString.h"
	#include "LiteralInt.h"
	#include "Type.h"
	#include "CallArgumentList.h"
	#include "Visibility.h"
	#include "Identifier.h"
	#include "ArgumentList.h"
	#include "VariableDefination.h"
	#include "FunctionCall.h"
	#include "Function.h"
	#include "WhileStatement.h"
	#include "IfStatement.h"
	#include "Statements.h"
	#include "Op2.h"
	#include "Return.h"
	#include "Block.h"
	#define YYSTYPE ASTNode*
}

%code top {
	#define YYERROR_VERBOSE 1
}

%initial-action {
	yylloc.first_line = 1;
	yylloc.last_column = 0;
}

%code {
	#include <fstream>

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
%token T_WHILE
%token T_DO
%token T_COMMA
%token T_ASSIGN
%token T_UNSIGNED
%token T_BYTE
%token T_SHORT
%token T_INT
%token T_CHAR
%token T_FLOAT
%token T_DOUBLE
%token visibility
%token T_RETURN

%left T_ADD T_SUB
%left T_MUL T_DIV
%left T_MOD
%left T_PWR

%%

compile_unit:
	function_defination {
		root = $1; }

expression:
	expression T_ADD expression {
		$$ = new Op2($1, Op2::ADD, $3); }
	| expression T_SUB expression {
		$$ = new Op2($1, Op2::SUB, $3); }
	| expression T_MUL expression {
		$$ = new Op2($1, Op2::MUL, $3); }
	| expression T_DIV expression {
		$$ = new Op2($1, Op2::DIV, $3); }
	| literal
	| identifier
	| function_call

literal:
	literal_int
	| literal_string

statement:
	T_SEMICOLON
	| expression T_SEMICOLON {
		$$ = $1; }
	| variable_defination T_SEMICOLON {
		$$ = $1; }
	| if_statement
	| while_statement
	| T_RETURN expression T_SEMICOLON {
		$$ = new Return($2); }
	| T_RETURN T_SEMICOLON {
		$$ = new Return(); }
	| T_BEGIN statements T_END {
		$$ = new Block((Statements *) $2); }
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

while_statement:
	T_WHILE T_LEFT_PARENTHESIS expression T_RIGHT_PARENTHESIS T_DO statement {
		$$ = new WhileStatement($3, $6); }

variable_defination:
	type_name {
		$$ = $1 = new VariableDefination($1); } variable_defination_list

variable_defination_list:
	identifier {
		((VariableDefination *) $0)->push_back($1); }
	| identifier T_ASSIGN expression {
		((VariableDefination *) $0)->push_back($1, $3); }
	| variable_defination_list T_COMMA identifier {
		((VariableDefination *) $0)->push_back($3); }
	| variable_defination_list T_COMMA identifier T_ASSIGN expression {
		((VariableDefination *) $0)->push_back($3, $5); }

type_name:
	base_type

base_type:
	T_UNSIGNED T_BYTE { 
		$$ = new Type(Type::BYTE, true); }
	| T_BYTE {
		$$ = new Type(Type::BYTE, false); }
	| T_UNSIGNED T_SHORT {
		$$ = new Type(Type::SHORT, true); }
	| T_SHORT {
		$$ = new Type(Type::SHORT, false); }
	| T_UNSIGNED T_INT {
		$$ = new Type(Type::INT, true); }
	| T_INT {
		$$ = new Type(Type::INT, false); }
	| T_CHAR {
		$$ = new Type(Type::CHAR); }
	| T_FLOAT {
		$$ = new Type(Type::FLOAT); }
	| T_DOUBLE {
		$$ = new Type(Type::DOUBLE); }

function_defination:
	visibility type_name identifier T_LEFT_PARENTHESIS arg_list T_RIGHT_PARENTHESIS T_BEGIN statements T_END {
		$$ = new Function((Visibility) (long long) $1, (Type*) $2, (Identifier*) $3, (ArgumentList*) $5, (Statements*) $8); }

arg_list:
	{ $$ = new ArgumentList(); }
	| type_name identifier {
		$$ = new ArgumentList((Type*) $1, (Identifier*) $2); }
	| arg_list T_COMMA type_name identifier {
		$$ = $1;
		((ArgumentList *) $$)->push_back((Type*) $3, (Identifier*) $4); }
		
function_call:
	identifier T_LEFT_PARENTHESIS call_arg_list T_RIGHT_PARENTHESIS {
		$$ = new FunctionCall($1, $3); }
	
call_arg_list:
	{ $$ = new CallArgumentList(); }
	| expression {
		$$ = new CallArgumentList($1); }
	| call_arg_list T_COMMA expression {
		$$ = $1;
		((CallArgumentList *) $$)->push_back($3); }

%%

void yyerror(const char *s) {
	if (input_filename) {
		std::ifstream ifs(input_filename);
		std::string line;
		for (int i = 0; i < yylloc.first_line; i++)
			getline(ifs, line);
		int len = fprintf(stderr, "%s:%-5d", input_filename, yylloc.first_line);
		for (const char *p = line.c_str(), *base = line.c_str(); *p; p++)
			if (*p == '\t')
				for (int i = 0; i < 4 - (p - base) % 4; i++)
					fprintf(stderr, " ");
			else
				fprintf(stderr, "%c", *p);
		fprintf(stderr, "\n");
		for (int i = -len; i < yylloc.first_column - 1; i++)
			fprintf(stderr, " ");
		for (int i = yylloc.first_column; i <= yylloc.last_column; i++)
			fprintf(stderr, "^");
		fprintf(stderr, "\n");
	}
	fprintf(stderr, "error: %s at %d:%d\n", s, yylloc.first_line, yylloc.first_column);
}
