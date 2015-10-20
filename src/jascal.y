%code requires {
	#include <list>
	
	#include "yyvaltypes.h"
	#include "Visibility.h"

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
}

%code top {
	#define YYERROR_VERBOSE 1
	
	#include "Type.h"
	#include "CallArgumentList.h"
	#include "Visibility.h"
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
}

%initial-action {
	yylloc.first_line = 1;
	yylloc.last_column = 0;
}

%union {
	ASTNode *ast;
	Expression *expression;
	Statements *statements;
	Type *type;
	CallArgumentList *call_arg_list;
	Identifier *identifier;
	Function *function;
	Visibility visibility;
	VariableDefination *var_def;
	ArrayDefinator *arr_def;
	ArrayAccessor *arr_acc;
	Namespace *ns;
	Module *module;
	std::list<ASTNode*> *list;
	std::list<std::pair<Identifier*,Expression*> > *var_entry_list;
	Class *cls;
	Interface *interface;
	std::list<std::pair<Type*, Identifier*> > *arg_list;
}

%code {
	#include <fstream>
	#include <list>

	int yylex (void);
	void yyerror (const char *);

	// global
	std::list<Module*> modules;

	// from main
	extern const char *input_filename;
}

%locations

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
%type <list> inmodule_definations
%type <cls> class_defination
%type <interface> interface_defination
%type <list> inclass_definations
%type <list> ininterface_definations
%type <expression> object_create

%token <expression> T_LITERAL_INT
%token <expression> T_LITERAL_STRING
%token <identifier> T_IDENTIFIER
%token T_MODULE
%token T_BEGIN
%token T_END
%token T_LEFT_CURLY
%token T_RIGHT_CURLY
%token T_SEMICOLON
%token T_IF
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
%token <visibility> T_VISIBILITY
%token T_RETURN
%token T_REPEAT
%token T_UNTIL
%token T_COLON
%token T_FUNCTION
%token T_PROCEDURE
%token T_DOTDOT
%token T_STRING
%token T_CLASS
%token T_NS
%token T_INTERFACE
%token T_VAR

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
%left T_LEFT_SQUARE T_RIGHT_SQUARE
%right T_NEW
%left T_DOT
%left T_LEFT_PARENTHESIS T_RIGHT_PARENTHESIS

%%

compile_unit:
	module_defination T_SEMICOLON {
		modules.push_back($1); }
	
module_defination:
	T_MODULE ns_identifier T_BEGIN inmodule_definations T_END {
		$$ = new Module($2, $4); }

ns_identifier:
	T_IDENTIFIER {
		$$ = new Namespace($1); }
	| ns_identifier T_NS T_IDENTIFIER {
		$$ = $1;
		$1->push_back($3); }
		
inmodule_definations:
	class_defination T_SEMICOLON {
		$$ = new std::list<ASTNode*>();
		$$->push_back((ASTNode *) $1); }
	| interface_defination T_SEMICOLON {
		$$ = new std::list<ASTNode*>();
		$$->push_back((ASTNode *) $1); }
	| inmodule_definations class_defination T_SEMICOLON {
		$$ = $1;
		$1->push_back((ASTNode *) $2); }
	| inmodule_definations interface_defination T_SEMICOLON  {
		$$ = $1;
		$1->push_back((ASTNode *) $2); }
	
class_defination:
	T_CLASS T_IDENTIFIER T_BEGIN inclass_definations T_END {
		$$ = new Class($2, $4); }
	
inclass_definations:
	function_defination T_SEMICOLON {
		$$ = new std::list<ASTNode*>();
		$$->push_back((ASTNode *) $1); }
	| T_VISIBILITY variable_defination T_SEMICOLON {
		$$ = new std::list<ASTNode*>();
		$$->push_back((ASTNode *) $2); }
	| inclass_definations function_defination T_SEMICOLON {
		$$ = $1;
		$1->push_back((ASTNode *) $2); }
	| inclass_definations T_VISIBILITY variable_defination T_SEMICOLON {
		$$ = $1;
		$1->push_back((ASTNode *) $3); }
	
interface_defination:
	T_INTERFACE T_IDENTIFIER T_BEGIN ininterface_definations T_END
	
ininterface_definations:
	function_declaration T_SEMICOLON {
		$$ = new std::list<ASTNode*>();
		$$->push_back((ASTNode *) $1); }
	| ininterface_definations function_declaration T_SEMICOLON {
		$$ = $1;
		$1->push_back((ASTNode *) $2); }
	
function_declaration:
	T_VISIBILITY T_FUNCTION T_IDENTIFIER T_LEFT_PARENTHESIS arg_list T_RIGHT_PARENTHESIS T_COLON type_name
	| T_VISIBILITY T_PROCEDURE T_IDENTIFIER T_LEFT_PARENTHESIS arg_list T_RIGHT_PARENTHESIS
		
expression:
	expression T_ADD expression {
		$$ = new Op2($1, Op2::ADD, $3); }
	| expression T_SUB expression {
		$$ = new Op2($1, Op2::SUB, $3); }
	| expression T_MUL expression {
		$$ = new Op2($1, Op2::MUL, $3); }
	| expression T_DIV expression {
		$$ = new Op2($1, Op2::DIV, $3); }
	| expression T_LT expression {
		$$ = new Op2($1, Op2::LT, $3); }
	| expression T_GT expression {
		$$ = new Op2($1, Op2::GT, $3); }
	| expression T_LEQ expression {
		$$ = new Op2($1, Op2::LEQ, $3); }
	| expression T_GEQ expression {
		$$ = new Op2($1, Op2::GEQ, $3); }
	| expression T_SELF_INC {
		$$ = new Op1($1, Op1::SELF_INC); }
	| expression T_SELF_DEC {
		$$ = new Op1($1, Op1::SELF_DEC); }
	| expression T_ASSIGN expression {
		$$ = new Op2($1, Op2::ASSIGN, $3); }
	| expression T_EQ expression {
		$$ = new Op2($1, Op2::EQ, $3); }
	| expression T_NEQ expression {
		$$ = new Op2($1, Op2::NEQ, $3); }
	| expression T_LOG_AND expression {
		$$ = new Op2($1, Op2::LOG_AND, $3); }
	| expression T_LOG_OR expression {
		$$ = new Op2($1, Op2::LOG_OR, $3); }
	| expression T_LOG_XOR expression {
		$$ = new Op2($1, Op2::LOG_XOR, $3); }
	| T_LEFT_PARENTHESIS expression T_RIGHT_PARENTHESIS {
		$$ = $2; }
	| literal
	| T_IDENTIFIER {
		$$ = $1; }
	| function_call
	| expression array_accessor {
		$$ = new ArrayAccess((Expression *) $1, (ArrayAccessor *) $2); }
	| object_create
	
object_create:
	T_NEW type_name {
		$$ = new New($2); }
	
array_accessor:
	T_LEFT_SQUARE array_accessor_list T_RIGHT_SQUARE {
		$$ = $2; }
		
array_accessor_list:
	{
		$$ = new ArrayAccessor(NULL); }
	| expression {
		$$ = new ArrayAccessor($1); }
	| array_accessor_list T_COMMA {
		$$ = $1;
		$1->push_back(NULL); }
	| array_accessor_list T_COMMA expression {
		$$ = $1;
		$1->push_back($3); }
		
array_definator:
	T_LEFT_SQUARE array_definator_list T_RIGHT_SQUARE {
		$$ = $2; }
		
array_definator_list:
	{
		$$ = new ArrayDefinator(NULL, NULL); }
	| expression T_DOTDOT expression {
		$$ = new ArrayDefinator($1, $3); }
	| expression T_DOTDOT {
		$$ = new ArrayDefinator($1, NULL); }
	| array_definator_list T_COMMA {
		$$ = $1;
		((ArrayDefinator *) $1)->push_back(NULL, NULL); }
	| array_definator_list T_COMMA expression T_DOTDOT expression {
		$$ = $1;
		((ArrayDefinator *) $1)->push_back($3, $5); }
	| array_definator_list T_COMMA expression T_DOTDOT {
		$$ = $1;
		((ArrayDefinator *) $1)->push_back($3, NULL); }

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
		$$ = new Return($2); }
	| T_RETURN {
		$$ = new Return(); }
	| repeat_statement
	| if_statement
	| T_BEGIN statement_list T_END {
		$$ = new Block((Statements *) $2); }

statement_list_head:
	{
		$$ = new Statements(); }
	| statement_list_head statement T_SEMICOLON {
		((Statements*) $1)->push_back($2);
		$$ = $1; }
		
statement_list:
	statement_list_head
	| statement_list_head statement {
		((Statements*) $1)->push_back($2);
		$$ = $1; }
		
if_statement:
	T_IF expression T_THEN statement {
		$$ = new IfStatement($2, $4); }
	| T_IF expression T_THEN statement T_ELSE statement {
		$$ = new IfStatement($2, $4, $6); }

while_statement:
	T_WHILE expression T_DO statement {
		$$ = new WhileStatement($2, $4); }

repeat_statement:
	T_REPEAT statement_list T_UNTIL expression {
		$$ = new RepeatStatement($2, $4); }
		
variable_defination:
	type_name variable_defination_list {
		$$ = new VariableDefination($1, $2); }

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
	| T_IDENTIFIER {
		$$ = new Type($1); }
	| type_name array_definator {
		$$ = new Type(Type::ARRAY, $1, $2); }

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
	| T_STRING {
		$$ = new Type(Type::STRING); }

function_defination:
	T_VISIBILITY T_FUNCTION T_IDENTIFIER T_LEFT_PARENTHESIS arg_list T_RIGHT_PARENTHESIS T_COLON type_name T_BEGIN statement_list T_END {
		$$ = new Function($1, $8, $3, $5, $10); }
	| T_VISIBILITY T_PROCEDURE T_IDENTIFIER T_LEFT_PARENTHESIS arg_list T_RIGHT_PARENTHESIS T_BEGIN statement_list T_END {
		$$ = new Function($1, NULL, $3, $5, $8); }

arg_list:
	{ $$ = new std::list<std::pair<Type*, Identifier*> >(); }
	| type_name T_IDENTIFIER {
		$$ = new std::list<std::pair<Type*, Identifier*> >();
		$$->push_back(std::pair<Type*, Identifier*>($1, $2)); }
	| arg_list T_COMMA type_name T_IDENTIFIER {
		$$ = $1;
		$$->push_back(std::pair<Type*, Identifier*>($3, $4)); }
		
function_call:
	T_IDENTIFIER T_LEFT_PARENTHESIS call_arg_list T_RIGHT_PARENTHESIS {
		$$ = new FunctionCall(NULL, $1, $3); }
	| expression T_DOT T_IDENTIFIER T_LEFT_PARENTHESIS call_arg_list T_RIGHT_PARENTHESIS {
		$$ = new FunctionCall($1, $3, $5); }
	
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
		int real_len = 0;
		const char *p, *base;
		for (p = line.c_str(), base = line.c_str(); p - base < yylloc.first_column - 1; p++)
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
		for (int i = yylloc.first_column; i <= yylloc.last_column; i++)
			fprintf(stderr, "^");
		fprintf(stderr, "\n");
	}
	fprintf(stderr, "error: %s at %d:%d\n", s, yylloc.first_line, yylloc.first_column);
}
