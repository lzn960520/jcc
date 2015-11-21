%define api.pure full
%locations
%define parse.error verbose
%define parse.lac full
%parse-param {Parser &parser} {CompileFile *&root}
%token-table
%verbose

%code requires {
	#include <list>

	#include "location.h"

	class ASTNode;
	class Expression;
	class Statements;
	class Type;
	class CallArgumentList;
	class Identifier;
	class Function;
	class VariableDefination;
	class ArrayDefinator;
	class ArrayAccessor;
	class Namespace;
	class Module;
	class Class;
	class Interface;
	class Qualifier;
	class MemberNode;
	class StructNode;
	class LiteralString;
	class Parser;
	class CompileFile;

	#define YYLTYPE Location
}

%code top {
	#include "Type.h"
	#include "CallArgumentList.h"
	#include "VariableDefination.h"
	#include "FunctionCall.h"
	#include "Function.h"
	#include "WhileStatement.h"
	#include "IfStatement.h"
	#include "Statements.h"
	#include "Op2.h"
	#include "Return.h"
	#include "Block.h"
	#include "RepeatStatement.h"
	#include "Op1.h"
	#include "ArrayAccess.h"
	#include "ArrayAccessor.h"
	#include "ArrayDefinator.h"
	#include "Namespace.h"
	#include "Module.h"
	#include "Class.h"
	#include "New.h"
	#include "Identifier.h"
	#include "Interface.h"
	#include "Qualifier.h"
	#include "MemberAccess.h"
	#include "MemberNode.h"
	#include "MemberVariableDefination.h"
	#include "exception.h"
	#include "LiteralString.h"
	#include "Parser.h"
	#include "CompileFile.h"
	#include "LiteralInt.h"
}

%union {
	ASTNode *ast;
	Expression *expression;
	Statements *statements;
	Type *type;
	CallArgumentList *call_arg_list;
	Identifier *identifier;
	Function *function;
	VariableDefination *var_def;
	ArrayDefinator *arr_def;
	ArrayAccessor *arr_acc;
	Namespace *ns;
	Module *module;
	std::list<StructNode*> *struct_list;
	std::list<std::pair<Identifier*,Expression*> > *var_entry_list;
	Class *cls;
	Interface *interface;
	std::list<std::pair<Type*, Identifier*> > *arg_list;
	std::list<MemberNode*> *member_list;
	Qualifier *qualifier;
	MemberNode *member;
	LiteralString *literal_string;
	std::string *string;
}

%code {
	#include <fstream>
	#include <list>

	#define yylex(a, b) (parser.yylex((a), (b)))
	void yyerror (YYLTYPE *, Parser &, CompileFile *&, const char *);

	#define GEN_LOC(a, x, y) { \
		(a).begin = (x).begin; \
		(a).end = (y).end; }
	#define YYLLOC_DEFAULT(Cur, Rhs, N) \
		do \
			if (N) { \
				(Cur).begin = YYRHSLOC(Rhs, 1).begin; \
				(Cur).end = YYRHSLOC(Rhs, N).end; \
			} else \
				(Cur).begin = (Cur).end = YYRHSLOC(Rhs, 0).end; \
		while (0)
	#define SAVE_LOC(a, b) { \
		(a)->loc = (b); }
}

%type <arr_def> array_definator
%type <arr_def> array_definator_list
%type <arr_acc> array_accessor
%type <arr_acc> array_accessor_list
%type <var_entry_list> variable_defination_list
%type <var_def> variable_defination
%type <function> function_defination
%type <function> function_declaration
%type <ast> while_statement if_statement repeat_statement statement
%type <statements> statement_list_head statement_list
%type <expression> expression literal function_call
%type <type> type_name base_type
%type <call_arg_list> call_arg_list
%type <arg_list> arg_list
%type <ns> ns_identifier
%type <module> module_defination
%type <struct_list> inmodule_definations
%type <cls> class_defination
%type <interface> interface_defination
%type <member_list> inclass_definations
%type <member_list> ininterface_definations
%type <expression> object_create
%type <qualifier> qualifier
%type <member> class_member_defination
%type <identifier> class_name
%type <string> using

%token <expression> T_LITERAL_INT "literal_int"
%token <literal_string> T_LITERAL_STRING "literal_string"
%token <identifier> T_IDENTIFIER "identifier"
%token T_MODULE "module" T_BEGIN "begin" T_END "end" T_LEFT_CURLY "{"
		T_RIGHT_CURLY "}" T_SEMICOLON ";" T_IF "if" T_THEN "then"
		T_ELSE "else" T_WHILE "while" T_DO "do" T_COMMA ","
		T_UNSIGNED "unsigned" T_BYTE "byte" T_VAR "var"	T_SHORT "short"
		T_INT "int" T_CHAR "char" T_FLOAT "float" T_DOUBLE "double"
		T_RETURN "return" T_INTERFACE "interface" T_REPEAT "repeat" 
		T_UNTIL "until" T_COLON ":" T_FUNCTION "function"
		T_PROCEDURE "procedure" T_DOTDOT ".." T_STRING "string"
		T_CLASS "class" T_NS "::" T_PUBLIC "public" T_PRIVATE "private"
		T_PROTECTED "protected" T_CONST "const" T_STATIC "static"
		T_USING "using" T_COMMENT "comment" T_TAB "\t" T_NEWLINE "\n"
		T_EXTENDS "extends" T_IMPLEMENTS "implements" T_ASSIGN "="
		T_LOG_OR "||" T_LOG_AND "&&" T_LOG_XOR "^^" T_LT "<" T_GT ">"
		T_LEQ "<=" T_GEQ ">=" T_EQ "==" T_NEQ "!=" T_LTGT "<>" T_ADD "+"
		T_SUB "-" T_MUL "*" T_DIV "/" T_MOD "%" T_PWR "**"
		T_BIT_OR_ASSIGN "|=" T_BIT_AND_ASSIGN "&=" T_BIT_XOR_ASSIGN "^="
		T_SELF_INC "++" T_SELF_DEC "--" T_LEFT_SQUARE "["
		T_RIGHT_SQUARE "]" T_LEFT_LEFT_SQUARE "[[" T_RIGHT_RIGHT_SQUARE "]]"
		T_NEW "new" T_DOT "." T_LEFT_PARENTHESIS "(" T_RIGHT_PARENTHESIS ")"

%right T_ASSIGN
%left T_LOG_OR
%left T_LOG_AND
%left T_LOG_XOR
%left T_LT T_GT T_LEQ T_GEQ T_EQ T_NEQ
%left T_ADD T_SUB
%left T_MUL T_DIV
%left T_MOD
%left T_PWR
%right T_BIT_OR_ASSIGN
%right T_BIT_AND_ASSIGN
%right T_BIT_XOR_ASSIGN
%left T_SELF_INC T_SELF_DEC
%left T_LEFT_SQUARE T_RIGHT_SQUARE T_LEFT_LEFT_SQUARE T_RIGHT_RIGHT_SQUARE
%right T_NEW
%left T_DOT
%left T_LEFT_PARENTHESIS T_RIGHT_PARENTHESIS

%%

compile_unit:
	using {
		root = new CompileFile(parser.getDirPath());
		root->addUsing(*$1);
		delete $1; }
    | module_defination {
		root = new CompileFile(parser.getDirPath());
		root->addModule($1); }
    | compile_unit using {
		root->addUsing(*$2);
		delete $2;
		SAVE_LOC(root, @$); }
    | compile_unit module_defination {
		root->addModule($2);
		SAVE_LOC(root, @$); }

using:
	T_USING T_LITERAL_STRING T_SEMICOLON {
		$$ = new std::string($2->text);
        delete $2; }

module_defination:
	T_MODULE ns_identifier T_BEGIN inmodule_definations T_END T_SEMICOLON {
		$$ = new Module($2, $4);
		SAVE_LOC($$, @$); }

ns_identifier:
	T_IDENTIFIER {
		$$ = new Namespace($1);
		GEN_LOC(@$, @1, @1);
		SAVE_LOC($$, @$); }
	| ns_identifier T_NS T_IDENTIFIER {
		$$ = $1;
		$1->push_back($3);
		GEN_LOC(@$, @1, @3);
		SAVE_LOC($$, @$); }

class_name:
	ns_identifier {
		$$ = new Identifier($1->getFullName().c_str());
		delete $1;
		SAVE_LOC($$, @$); }

inmodule_definations:
	class_defination T_SEMICOLON {
		$$ = new std::list<StructNode*>();
		$$->push_back($1); }
	| interface_defination T_SEMICOLON {
		$$ = new std::list<StructNode*>();
		$$->push_back($1); }
	| inmodule_definations class_defination T_SEMICOLON {
		$$ = $1;
		$1->push_back($2); }
	| inmodule_definations interface_defination T_SEMICOLON  {
		$$ = $1;
		$1->push_back($2); }

class_defination:
	T_CLASS T_IDENTIFIER class_implements T_BEGIN inclass_definations T_END {
		$$ = new Class($2, $5);
		SAVE_LOC($$, @$); }
	| T_CLASS T_IDENTIFIER T_EXTENDS class_name class_implements T_BEGIN inclass_definations T_END {
		$$ = new Class($2, $7);
		SAVE_LOC($$, @$); }

class_implements:
	{
		}
	| T_IMPLEMENTS implement_list {
		}

implement_list:
	class_name {
		}
	| implement_list T_COMMA class_name {
		}

qualifier:
	T_PUBLIC {
		$$ = new Qualifier();
		$$->setPublic();
		SAVE_LOC($$, @$); }
	| T_PRIVATE {
		$$ = new Qualifier();
		$$->setPrivate();
		SAVE_LOC($$, @$); }
	| T_PROTECTED {
		$$ = new Qualifier();
		$$->setProtected();
		SAVE_LOC($$, @$); }
	| T_CONST {
		$$ = new Qualifier();
		$$->setConst();
		SAVE_LOC($$, @$); }
	| T_STATIC {
		$$ = new Qualifier();
		$$->setStatic();
		SAVE_LOC($$, @$); }
	| qualifier T_PUBLIC {
		$$ = $1;
		$$->setPublic();
		SAVE_LOC($$, @$); }
	| qualifier T_PRIVATE {
		$$ = $1;
		$$->setPrivate();
		SAVE_LOC($$, @$); }
	| qualifier T_PROTECTED {
		$$ = $1;
		$$->setProtected();
		SAVE_LOC($$, @$); }
	| qualifier T_CONST {
		$$ = $1;
		$$->setConst();
		SAVE_LOC($$, @$); }
	| qualifier T_STATIC {
		$$ = $1;
		$$->setStatic();
		SAVE_LOC($$, @$); }

inclass_definations:
	{
		$$ = new std::list<MemberNode*>(); }
	| class_member_defination {
		$$ = new std::list<MemberNode*>();
		$$->push_back($1); }
	| inclass_definations class_member_defination {
		$$ = $1;
		$1->push_back($2); }

class_member_defination:
	function_defination T_SEMICOLON {
		$$ = $1; }
	| qualifier variable_defination T_SEMICOLON {
		$$ = new MemberVariableDefination($1, $2);
		SAVE_LOC($$, @$); }

interface_defination:
	T_INTERFACE T_IDENTIFIER T_BEGIN ininterface_definations T_END

ininterface_definations:
	{
		$$ = new std::list<MemberNode*>(); }
	| function_declaration T_SEMICOLON {
		$$ = new std::list<MemberNode*>();
		$$->push_back($1); }
	| ininterface_definations function_declaration T_SEMICOLON {
		$$ = $1;
		$1->push_back($2); }

function_declaration:
	qualifier T_FUNCTION T_IDENTIFIER T_LEFT_PARENTHESIS arg_list T_RIGHT_PARENTHESIS T_COLON type_name
	| qualifier T_PROCEDURE T_IDENTIFIER T_LEFT_PARENTHESIS arg_list T_RIGHT_PARENTHESIS

expression:
	expression T_ADD expression {
		$$ = new Op2($1, Op2::ADD, $3);
		SAVE_LOC($$, @2); }
	| expression T_SUB expression {
		$$ = new Op2($1, Op2::SUB, $3);
		SAVE_LOC($$, @2); }
	| expression T_MUL expression {
		$$ = new Op2($1, Op2::MUL, $3);
		SAVE_LOC($$, @2); }
	| expression T_DIV expression {
		$$ = new Op2($1, Op2::DIV, $3);
 		SAVE_LOC($$, @2); }
	| expression T_LT expression {
		$$ = new Op2($1, Op2::LT, $3);
		SAVE_LOC($$, @2); }
	| expression T_GT expression {
		$$ = new Op2($1, Op2::GT, $3);
		SAVE_LOC($$, @2); }
	| expression T_LEQ expression {
		$$ = new Op2($1, Op2::LEQ, $3);
		SAVE_LOC($$, @2); }
	| expression T_GEQ expression {
		$$ = new Op2($1, Op2::GEQ, $3);
		SAVE_LOC($$, @2); }
	| expression T_SELF_INC {
		$$ = new Op1($1, Op1::SELF_INC);
		SAVE_LOC($$, @2); }
	| expression T_SELF_DEC {
		$$ = new Op1($1, Op1::SELF_DEC);
		SAVE_LOC($$, @2); }
	| expression T_ASSIGN expression {
		$$ = new Op2($1, Op2::ASSIGN, $3);
		SAVE_LOC($$, @2); }
	| expression T_EQ expression {
		$$ = new Op2($1, Op2::EQ, $3);
		SAVE_LOC($$, @2); }
	| expression T_NEQ expression {
		$$ = new Op2($1, Op2::NEQ, $3);
		SAVE_LOC($$, @2); }
	| expression T_LOG_AND expression {
		$$ = new Op2($1, Op2::LOG_AND, $3);
		SAVE_LOC($$, @2); }
	| expression T_LOG_OR expression {
		$$ = new Op2($1, Op2::LOG_OR, $3);
		SAVE_LOC($$, @2); }
	| expression T_LOG_XOR expression {
		$$ = new Op2($1, Op2::LOG_XOR, $3);
		SAVE_LOC($$, @2); }
	| T_LEFT_PARENTHESIS expression T_RIGHT_PARENTHESIS {
		$$ = $2; }
	| literal
	| T_IDENTIFIER {
		$$ = $1; }
	| function_call
	| expression array_accessor {
		$$ = new ArrayAccess($1, $2);
		SAVE_LOC($$, @2); }
	| object_create
	| expression T_DOT T_IDENTIFIER {
		$$ = new MemberAccess($1, $3);
		SAVE_LOC($$, @2); }
	| T_LEFT_LEFT_SQUARE tuple_list T_RIGHT_RIGHT_SQUARE {
		}

tuple_list:
	expression {
		}
	| tuple_list T_COMMA expression {
		}

object_create:
	T_NEW type_name {
		$$ = new New($2);
		SAVE_LOC($$, @$); }

array_accessor:
	T_LEFT_SQUARE array_accessor_list T_RIGHT_SQUARE {
		$$ = $2;
		SAVE_LOC($$, @$); }

array_accessor_list:
	expression {
		$$ = new ArrayAccessor($1);
		SAVE_LOC($$, @$); }
	| array_accessor_list T_COMMA expression {
		$$ = $1;
		$1->push_back($3);
		SAVE_LOC($$, @$); }

array_definator:
	T_LEFT_SQUARE array_definator_list T_RIGHT_SQUARE {
		$$ = $2;
		SAVE_LOC($$, @$); }

array_definator_list:
	{
		$$ = new ArrayDefinator(new LiteralInt(0, false), NULL);
		SAVE_LOC($$, @$); }
	| expression T_DOTDOT expression {
		$$ = new ArrayDefinator($1, $3);
		SAVE_LOC($$, @$); }
	| expression T_DOTDOT {
		$$ = new ArrayDefinator($1, NULL);
		SAVE_LOC($$, @$); }
	| array_definator_list T_COMMA {
		$$ = $1;
		$1->push_back(new LiteralInt(0, false), NULL);
		SAVE_LOC($$, @$); }
	| array_definator_list T_COMMA expression T_DOTDOT expression {
		$$ = $1;
		$1->push_back($3, $5);
		SAVE_LOC($$, @$); }
	| array_definator_list T_COMMA expression T_DOTDOT {
		$$ = $1;
		$1->push_back($3, NULL);
		SAVE_LOC($$, @$); }

literal:
	T_LITERAL_INT
	| T_LITERAL_STRING

statement:
	expression {
		$$ = $1; }
	| while_statement
	| T_VAR variable_defination {
		$$ = $2; }
	| T_RETURN expression {
		$$ = new Return($2);
		SAVE_LOC($$, @$); }
	| T_RETURN {
		$$ = new Return();
		SAVE_LOC($$, @$); }
	| repeat_statement
	| if_statement
	| T_BEGIN statement_list T_END {
		$$ = new Block($2);
		SAVE_LOC($$, @$); }

statement_list_head:
	{
		$$ = new Statements();
		SAVE_LOC($$, @$); }
	| statement_list_head statement T_SEMICOLON {
		$1->push_back($2);
		$$ = $1;
		SAVE_LOC($$, @$); }

statement_list:
	statement_list_head
	| statement_list_head statement {
		$1->push_back($2);
		$$ = $1;
		SAVE_LOC($$, @$); }

if_statement:
	T_IF expression T_THEN statement {
		$$ = new IfStatement($2, $4);
		SAVE_LOC($$, @$); }
	| T_IF expression T_THEN statement T_ELSE statement {
		$$ = new IfStatement($2, $4, $6);
		SAVE_LOC($$, @$); }

while_statement:
	T_WHILE expression T_DO statement {
		$$ = new WhileStatement($2, $4);
		SAVE_LOC($$, @$); }

repeat_statement:
	T_REPEAT statement_list T_UNTIL expression {
		$$ = new RepeatStatement($2, $4);
		SAVE_LOC($$, @$); }

variable_defination:
	type_name variable_defination_list {
		$$ = new VariableDefination($1, $2);
		SAVE_LOC($$, @$); }

variable_defination_list:
	T_IDENTIFIER {
		$$ = new std::list<std::pair<Identifier*, Expression*> >();
		$$->push_back(std::pair<Identifier*, Expression*>($1, NULL)); }
	| T_IDENTIFIER T_ASSIGN expression {
		$$ = new std::list<std::pair<Identifier*, Expression*> >();
		$$->push_back(std::pair<Identifier*, Expression*>($1, $3)); }
	| variable_defination_list T_COMMA T_IDENTIFIER {
		$$ = $1;
		$$->push_back(std::pair<Identifier*, Expression*>($3, NULL)); }
	| variable_defination_list T_COMMA T_IDENTIFIER T_ASSIGN expression {
		$$ = $1;
		$$->push_back(std::pair<Identifier*, Expression*>($3, $5)); }

type_name:
	base_type
	| class_name {
		$$ = new Type($1);
		SAVE_LOC($$, @$); }
	| type_name array_definator {
		$$ = new Type($1, $2);
		SAVE_LOC($$, @$); }

base_type:
	T_UNSIGNED T_BYTE {
		$$ = new Type(Type::BYTE, true);
		SAVE_LOC($$, @$); }
	| T_BYTE {
		$$ = new Type(Type::BYTE, false);
		SAVE_LOC($$, @$); }
	| T_UNSIGNED T_SHORT {
		$$ = new Type(Type::SHORT, true);
		SAVE_LOC($$, @$); }
	| T_SHORT {
		$$ = new Type(Type::SHORT, false);
		SAVE_LOC($$, @$); }
	| T_UNSIGNED T_INT {
		$$ = new Type(Type::INT, true);
		SAVE_LOC($$, @$); }
	| T_INT {
		$$ = new Type(Type::INT, false);
		SAVE_LOC($$, @$); }
	| T_CHAR {
		$$ = new Type(Type::CHAR);
		SAVE_LOC($$, @$); }
	| T_FLOAT {
		$$ = new Type(Type::FLOAT);
		SAVE_LOC($$, @$); }
	| T_DOUBLE {
		$$ = new Type(Type::DOUBLE);
		SAVE_LOC($$, @$); }
	| T_STRING {
		$$ = new Type(Type::STRING);
		SAVE_LOC($$, @$); }

function_defination:
	qualifier T_FUNCTION T_IDENTIFIER T_LEFT_PARENTHESIS arg_list T_RIGHT_PARENTHESIS T_COLON type_name T_BEGIN statement_list T_END {
		$$ = new Function($1, $8, $3, $5, $10);
		SAVE_LOC($$, @$); }
	| qualifier T_PROCEDURE T_IDENTIFIER T_LEFT_PARENTHESIS arg_list T_RIGHT_PARENTHESIS T_BEGIN statement_list T_END {
		$$ = new Function($1, NULL, $3, $5, $8);
		SAVE_LOC($$, @$); }

arg_list:
	{
		$$ = new std::list<std::pair<Type*, Identifier*> >(); }
	| type_name T_IDENTIFIER {
		$$ = new std::list<std::pair<Type*, Identifier*> >();
		$$->push_back(std::pair<Type*, Identifier*>($1, $2)); }
	| arg_list T_COMMA type_name T_IDENTIFIER {
		$$ = $1;
		$$->push_back(std::pair<Type*, Identifier*>($3, $4)); }

function_call:
	T_IDENTIFIER T_LEFT_PARENTHESIS call_arg_list T_RIGHT_PARENTHESIS {
		$$ = new FunctionCall(NULL, $1, $3);
		SAVE_LOC($$, @$); }
	| expression T_DOT T_IDENTIFIER T_LEFT_PARENTHESIS call_arg_list T_RIGHT_PARENTHESIS {
		$$ = new FunctionCall($1, $3, $5);
		SAVE_LOC($$, @$); }

call_arg_list:
	{ $$ = new CallArgumentList();
		SAVE_LOC($$, @$); }
	| expression {
		$$ = new CallArgumentList($1);
		SAVE_LOC($$, @$); }
	| call_arg_list T_COMMA expression {
		$$ = $1;
		$$->push_back($3);
		SAVE_LOC($$, @$); }

%%

const char * const *tokenNames = yytname;

void yyerror(YYLTYPE *yylloc, Parser &, CompileFile *&, const char *s) {
	if (!yylloc->begin.filename.empty()) {
		std::ifstream ifs(yylloc->begin.filename.c_str());
		std::string line;
		for (int i = 0; i < yylloc->begin.line; i++)
			getline(ifs, line);
		int len = fprintf(stderr, "%s:%-5d", yylloc->begin.filename.c_str(), yylloc->begin.line);
		int real_len = 0;
		const char *p, *base;
		for (p = line.c_str(), base = line.c_str(); p - base < yylloc->begin.column - 1; p++)
			if (*p == '\t')
				for (int i = 0; i < 4 - (p - base) % 4; i++) {
					fprintf(stderr, " ");
					real_len++;
				}
			else {
				fprintf(stderr, "%c", *p);
				real_len++;
			}
		for (; *p; p++)
			if (*p == '\t')
				for (int i = 0; i < 4 - (p - base) % 4; i++)
					fprintf(stderr, " ");
			else
				fprintf(stderr, "%c", *p);
		fprintf(stderr, "\n");
		for (int i = -len; i < real_len; i++)
			fprintf(stderr, " ");
		for (int i = yylloc->begin.column; i <= yylloc->end.column; i++)
			fprintf(stderr, "^");
		fprintf(stderr, "\n");
	}
	fprintf(stderr, "error: %s at %d:%d\n", s, yylloc->begin.line, yylloc->begin.column);
	throw CompileException("syntax error");
}
