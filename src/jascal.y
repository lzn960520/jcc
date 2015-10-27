%code requires {
	#include <list>
	
	#include "yyvaltypes.h"

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
}

%code top {
	#define YYERROR_VERBOSE 1
	
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
	VariableDefination *var_def;
	ArrayDefinator *arr_def;
	ArrayAccessor *arr_acc;
	Namespace *ns;
	Module *module;
	struct { YYLTYPE loc; std::list<ASTNode*> *v; } list;
	struct { YYLTYPE loc; std::list<std::pair<Identifier*,Expression*> > *v; } var_entry_list;
	Class *cls;
	Interface *interface;
	struct { YYLTYPE loc; std::list<std::pair<Type*, Identifier*> > *v; } arg_list;
	YYLTYPE token;
	Qualifier *qualifier;
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

	#define GEN_LOC(a, x, y) { \
		(a).first_line = (x).first_line; \
		(a).first_column = (x).first_column; \
		(a).last_line = (y).last_line; \
		(a).last_column = (y).last_column; }
	#define P(x) x->loc
	#define D(x) x
	#define S(x) x.loc
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
%type <qualifier> qualifier

%token <expression> T_LITERAL_INT
%token <expression> T_LITERAL_STRING
%token <identifier> T_IDENTIFIER
%token <token> T_MODULE T_BEGIN T_END T_LEFT_CURLY T_RIGHT_CURLY T_SEMICOLON
		T_IF T_THEN T_ELSE T_WHILE T_DO T_COMMA T_UNSIGNED T_BYTE T_VAR
		T_SHORT T_INT T_CHAR T_FLOAT T_DOUBLE T_RETURN T_INTERFACE
		T_REPEAT T_UNTIL T_COLON T_FUNCTION T_PROCEDURE T_DOTDOT T_STRING
		T_CLASS T_NS T_PUBLIC T_PRIVATE T_PROTECTED T_CONST T_STATIC

%right <token> T_ASSIGN
%left <token> T_LOG_OR
%left <token> T_LOG_AND
%left <token> T_LOG_XOR
%left <token> T_LT T_GT T_LEQ T_GEQ T_EQ T_NEQ
%left <token> T_ADD T_SUB
%left <token> T_MUL T_DIV
%left <token> T_MOD
%left <token> T_PWR
%right <token> T_BIT_OR_ASSIGN
%right <token> T_BIT_AND_ASSIGN
%right <token> T_BIT_XOR_ASSIGN
%left <token> T_SELF_INC T_SELF_DEC
%left <token> T_LEFT_SQUARE T_RIGHT_SQUARE
%right <token> T_NEW
%left <token> T_DOT
%left <token> T_LEFT_PARENTHESIS T_RIGHT_PARENTHESIS

%%

compile_unit:
	module_defination T_SEMICOLON {
		modules.push_back($1); }
	
module_defination:
	T_MODULE ns_identifier T_BEGIN inmodule_definations T_END {
		$$ = new Module($2, $4.v);
		GEN_LOC(P($$), D($1), D($5)); }

ns_identifier:
	T_IDENTIFIER {
		$$ = new Namespace($1);
		GEN_LOC(P($$), P($1), P($1)); }
	| ns_identifier T_NS T_IDENTIFIER {
		$$ = $1;
		$1->push_back($3);
		GEN_LOC(P($$), P($1), P($3)); }
		
inmodule_definations:
	class_defination T_SEMICOLON {
		$$.v = new std::list<ASTNode*>();
		$$.v->push_back((ASTNode *) $1);
		GEN_LOC(S($$), P($1), D($2)); }
	| interface_defination T_SEMICOLON {
		$$.v = new std::list<ASTNode*>();
		$$.v->push_back((ASTNode *) $1); 
		GEN_LOC(S($$), P($1), D($2)); }
	| inmodule_definations class_defination T_SEMICOLON {
		$$.v = $1.v;
		$1.v->push_back((ASTNode *) $2);
		GEN_LOC(S($$), S($1), D($3)); }
	| inmodule_definations interface_defination T_SEMICOLON  {
		$$.v = $1.v;
		$1.v->push_back((ASTNode *) $2);
		GEN_LOC(S($$), S($1), D($3)); }
	
class_defination:
	T_CLASS T_IDENTIFIER T_BEGIN inclass_definations T_END {
		$$ = new Class($2, $4.v);
		GEN_LOC(P($$), D($1), D($5)); }

qualifier:
	T_PUBLIC {
		$$ = new Qualifier();
		$$->setPublic();
		GEN_LOC(P($$), D($1), D($1)); }
	| T_PRIVATE {
		$$ = new Qualifier();
		$$->setPrivate();
		GEN_LOC(P($$), D($1), D($1)); }
	| T_PROTECTED {
		$$ = new Qualifier();
		$$->setProtected();
		GEN_LOC(P($$), D($1), D($1)); }
	| T_CONST {
		$$ = new Qualifier();
		$$->setConst();
		GEN_LOC(P($$), D($1), D($1)); }
	| T_STATIC {
		$$ = new Qualifier();
		$$->setStatic();
		GEN_LOC(P($$), D($1), D($1)); }
	| qualifier T_PUBLIC {
		$$ = $1;
		$$->setPublic();
		GEN_LOC(P($$), P($1), D($2)); }
	| qualifier T_PRIVATE {
		$$ = $1;
		$$->setPrivate();
		GEN_LOC(P($$), P($1), D($2)); }
	| qualifier T_PROTECTED {
		$$ = $1;
		$$->setProtected();
		GEN_LOC(P($$), P($1), D($2)); }
	| qualifier T_CONST {
		$$ = $1;
		$$->setConst();
		GEN_LOC(P($$), P($1), D($2)); }
	| qualifier T_STATIC {
		$$ = $1;
		$$->setStatic();
		GEN_LOC(P($$), P($1), D($2)); }
	
inclass_definations:
	function_defination T_SEMICOLON {
		$$.v = new std::list<ASTNode*>();
		$$.v->push_back((ASTNode *) $1);
		GEN_LOC(S($$), P($1), D($2)); }
	| qualifier variable_defination T_SEMICOLON {
		$$.v = new std::list<ASTNode*>();
		$$.v->push_back((ASTNode *) $2);
		GEN_LOC(S($$), P($1), D($3)); }
	| inclass_definations function_defination T_SEMICOLON {
		$$.v = $1.v;
		$1.v->push_back((ASTNode *) $2);
		GEN_LOC(S($$), S($1), D($3)); }
	| inclass_definations qualifier variable_defination T_SEMICOLON {
		$$.v = $1.v;
		$1.v->push_back((ASTNode *) $3);
		GEN_LOC(S($$), S($1), D($4)); }
	
interface_defination:
	T_INTERFACE T_IDENTIFIER T_BEGIN ininterface_definations T_END
	
ininterface_definations:
	function_declaration T_SEMICOLON {
		$$.v = new std::list<ASTNode*>();
		$$.v->push_back((ASTNode *) $1);
		GEN_LOC(S($$), P($1), D($2)); }
	| ininterface_definations function_declaration T_SEMICOLON {
		$$.v = $1.v;
		$1.v->push_back((ASTNode *) $2);
		GEN_LOC(S($$), S($1), D($3)); }
	
function_declaration:
	qualifier T_FUNCTION T_IDENTIFIER T_LEFT_PARENTHESIS arg_list T_RIGHT_PARENTHESIS T_COLON type_name
	| qualifier T_PROCEDURE T_IDENTIFIER T_LEFT_PARENTHESIS arg_list T_RIGHT_PARENTHESIS
		
expression:
	expression T_ADD expression {
		$$ = new Op2($1, Op2::ADD, $3);
		GEN_LOC(P($$), D($2), D($2)); }
	| expression T_SUB expression {
		$$ = new Op2($1, Op2::SUB, $3);
		GEN_LOC(P($$), D($2), D($2)); }
	| expression T_MUL expression {
		$$ = new Op2($1, Op2::MUL, $3);
		GEN_LOC(P($$), D($2), D($2)); }
	| expression T_DIV expression {
		$$ = new Op2($1, Op2::DIV, $3);
 		GEN_LOC(P($$), D($2), D($2)); }
	| expression T_LT expression {
		$$ = new Op2($1, Op2::LT, $3);
		GEN_LOC(P($$), D($2), D($2)); }
	| expression T_GT expression {
		$$ = new Op2($1, Op2::GT, $3);
		GEN_LOC(P($$), D($2), D($2)); }
	| expression T_LEQ expression {
		$$ = new Op2($1, Op2::LEQ, $3);
		GEN_LOC(P($$), D($2), D($2)); }
	| expression T_GEQ expression {
		$$ = new Op2($1, Op2::GEQ, $3);
		GEN_LOC(P($$), D($2), D($2)); }
	| expression T_SELF_INC {
		$$ = new Op1($1, Op1::SELF_INC);
		GEN_LOC(P($$), D($2), D($2)); }
	| expression T_SELF_DEC {
		$$ = new Op1($1, Op1::SELF_DEC);
		GEN_LOC(P($$), D($2), D($2)); }
	| expression T_ASSIGN expression {
		$$ = new Op2($1, Op2::ASSIGN, $3);
		GEN_LOC(P($$), D($2), D($2)); }
	| expression T_EQ expression {
		$$ = new Op2($1, Op2::EQ, $3);
		GEN_LOC(P($$), D($2), D($2)); }
	| expression T_NEQ expression {
		$$ = new Op2($1, Op2::NEQ, $3);
		GEN_LOC(P($$), D($2), D($2)); }
	| expression T_LOG_AND expression {
		$$ = new Op2($1, Op2::LOG_AND, $3);
		GEN_LOC(P($$), D($2), D($2)); }
	| expression T_LOG_OR expression {
		$$ = new Op2($1, Op2::LOG_OR, $3);
		GEN_LOC(P($$), D($2), D($2)); }
	| expression T_LOG_XOR expression {
		$$ = new Op2($1, Op2::LOG_XOR, $3);
		GEN_LOC(P($$), D($2), D($2)); }
	| T_LEFT_PARENTHESIS expression T_RIGHT_PARENTHESIS {
		$$ = $2; }
	| literal
	| T_IDENTIFIER {
		$$ = $1; }
	| function_call
	| expression array_accessor {
		$$ = new ArrayAccess($1, $2);
		GEN_LOC(P($$), P($2), P($2)); }
	| object_create
	| expression T_DOT T_IDENTIFIER {
		$$ = new MemberAccess($1, $3);
		GEN_LOC(P($$), D($2), D($2)); }
	
object_create:
	T_NEW type_name {
		$$ = new New($2);
		GEN_LOC(P($$), D($1), P($2)); }
	
array_accessor:
	T_LEFT_SQUARE array_accessor_list T_RIGHT_SQUARE {
		$$ = $2;
		GEN_LOC(P($$), D($1), D($3)); }
		
array_accessor_list:
	{
		$$ = new ArrayAccessor(NULL);
		$$->loc = yylloc; }
	| expression {
		$$ = new ArrayAccessor($1);
		GEN_LOC(P($$), P($1), P($1)); }
	| array_accessor_list T_COMMA expression {
		$$ = $1;
		$1->push_back($3);
		GEN_LOC(P($$), P($1), P($3)); }
		
array_definator:
	T_LEFT_SQUARE array_definator_list T_RIGHT_SQUARE {
		$$ = $2; }
		
array_definator_list:
	{
		$$ = new ArrayDefinator(NULL, NULL);
		$$->loc = yylloc; }
	| expression T_DOTDOT expression {
		$$ = new ArrayDefinator($1, $3);
		GEN_LOC(P($$), P($1), P($3)); }
	| expression T_DOTDOT {
		$$ = new ArrayDefinator($1, NULL);
		GEN_LOC(P($$), P($1), D($2)); }
	| array_definator_list T_COMMA {
		$$ = $1;
		$1->push_back(NULL, NULL);
		GEN_LOC(P($$), P($1), D($2)); }
	| array_definator_list T_COMMA expression T_DOTDOT expression {
		$$ = $1;
		$1->push_back($3, $5);
		GEN_LOC(P($$), P($1), P($5)); }
	| array_definator_list T_COMMA expression T_DOTDOT {
		$$ = $1;
		$1->push_back($3, NULL);
		GEN_LOC(P($$), P($1), D($4)); }

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
		GEN_LOC(P($$), D($1), P($2)); }
	| T_RETURN {
		$$ = new Return();
		GEN_LOC(P($$), D($1), D($1)); }
	| repeat_statement
	| if_statement
	| T_BEGIN statement_list T_END {
		$$ = new Block($2);
		GEN_LOC(P($$), D($1), D($3)); }

statement_list_head:
	{
		$$ = new Statements();
		$$->loc = yylloc; }
	| statement_list_head statement T_SEMICOLON {
		$1->push_back($2);
		$$ = $1;
		GEN_LOC(P($$), P($1), D($3)); }
		
statement_list:
	statement_list_head
	| statement_list_head statement {
		((Statements*) $1)->push_back($2);
		$$ = $1;
		GEN_LOC(P($$), P($1), P($2)); }

if_statement:
	T_IF expression T_THEN statement {
		$$ = new IfStatement($2, $4);
		GEN_LOC(P($$), D($1), P($4)); }
	| T_IF expression T_THEN statement T_ELSE statement {
		$$ = new IfStatement($2, $4, $6);
		GEN_LOC(P($$), D($1), P($6)); }

while_statement:
	T_WHILE expression T_DO statement {
		$$ = new WhileStatement($2, $4);
		GEN_LOC(P($$), D($1), P($4)); }

repeat_statement:
	T_REPEAT statement_list T_UNTIL expression {
		$$ = new RepeatStatement($2, $4);
		GEN_LOC(P($$), D($1), P($4)); }
		
variable_defination:
	type_name variable_defination_list {
		$$ = new VariableDefination($1, $2.v);
		GEN_LOC(P($$), P($1), S($2)); }

variable_defination_list:
	T_IDENTIFIER {
		$$.v = new std::list<std::pair<Identifier*, Expression*> >();
		$$.v->push_back(std::pair<Identifier*, Expression*>($1, NULL));
		GEN_LOC(S($$), P($1), P($1)); }
	| T_IDENTIFIER T_ASSIGN expression {
		$$.v = new std::list<std::pair<Identifier*, Expression*> >();
		$$.v->push_back(std::pair<Identifier*, Expression*>($1, $3));
		GEN_LOC(S($$), P($1), P($3)); }
	| variable_defination_list T_COMMA T_IDENTIFIER {
		$$.v = $1.v;
		$$.v->push_back(std::pair<Identifier*, Expression*>($3, NULL));
		GEN_LOC(S($$), S($1), P($3)); }
	| variable_defination_list T_COMMA T_IDENTIFIER T_ASSIGN expression {
		$$.v = $1.v;
		$$.v->push_back(std::pair<Identifier*, Expression*>($3, $5));
		GEN_LOC(S($$), S($1), P($5)); }

type_name:
	base_type
	| T_IDENTIFIER {
		$$ = new Type($1);
		GEN_LOC(P($$), P($1), P($1)); }
	| type_name array_definator {
		$$ = new Type(Type::ARRAY, $1, $2);
		GEN_LOC(P($$), P($1), P($2)); }

base_type:
	T_UNSIGNED T_BYTE { 
		$$ = new Type(Type::BYTE, true);
		GEN_LOC(P($$), D($1), D($2)); }
	| T_BYTE {
		$$ = new Type(Type::BYTE, false);
		GEN_LOC(P($$), D($1), D($1)); }
	| T_UNSIGNED T_SHORT {
		$$ = new Type(Type::SHORT, true);
		GEN_LOC(P($$), D($1), D($2)); }
	| T_SHORT {
		$$ = new Type(Type::SHORT, false);
		GEN_LOC(P($$), D($1), D($1)); }
	| T_UNSIGNED T_INT {
		$$ = new Type(Type::INT, true);
		GEN_LOC(P($$), D($1), D($2)); }
	| T_INT {
		$$ = new Type(Type::INT, false);
		GEN_LOC(P($$), D($1), D($1)); }
	| T_CHAR {
		$$ = new Type(Type::CHAR);
		GEN_LOC(P($$), D($1), D($1)); }
	| T_FLOAT {
		$$ = new Type(Type::FLOAT);
		GEN_LOC(P($$), D($1), D($1)); }
	| T_DOUBLE {
		$$ = new Type(Type::DOUBLE);
		GEN_LOC(P($$), D($1), D($1)); }
	| T_STRING {
		$$ = new Type(Type::STRING);
		GEN_LOC(P($$), D($1), D($1)); }

function_defination:
	qualifier T_FUNCTION T_IDENTIFIER T_LEFT_PARENTHESIS arg_list T_RIGHT_PARENTHESIS T_COLON type_name T_BEGIN statement_list T_END {
		$$ = new Function($1, $8, $3, $5.v, $10);
		GEN_LOC(P($$), P($1), D($11)); }
	| qualifier T_PROCEDURE T_IDENTIFIER T_LEFT_PARENTHESIS arg_list T_RIGHT_PARENTHESIS T_BEGIN statement_list T_END {
		$$ = new Function($1, NULL, $3, $5.v, $8);
		GEN_LOC(P($$), P($1), D($9)); }

arg_list:
	{
		$$.v = new std::list<std::pair<Type*, Identifier*> >();
		$$.loc = yylloc; }
	| type_name T_IDENTIFIER {
		$$.v = new std::list<std::pair<Type*, Identifier*> >();
		$$.v->push_back(std::pair<Type*, Identifier*>($1, $2));
		GEN_LOC(S($$), P($1), P($2)); }
	| arg_list T_COMMA type_name T_IDENTIFIER {
		$$.v = $1.v;
		$$.v->push_back(std::pair<Type*, Identifier*>($3, $4));
		GEN_LOC(S($$), S($1), P($4)); }
		
function_call:
	T_IDENTIFIER T_LEFT_PARENTHESIS call_arg_list T_RIGHT_PARENTHESIS {
		$$ = new FunctionCall(NULL, $1, $3);
		GEN_LOC(P($$), P($1), D($4)); }
	| expression T_DOT T_IDENTIFIER T_LEFT_PARENTHESIS call_arg_list T_RIGHT_PARENTHESIS {
		$$ = new FunctionCall($1, $3, $5);
		GEN_LOC(P($$), P($1), D($6)); }
	
call_arg_list:
	{ $$ = new CallArgumentList();
		$$->loc = yylloc; }
	| expression {
		$$ = new CallArgumentList($1);
		GEN_LOC(P($$), P($1), P($1)); }
	| call_arg_list T_COMMA expression {
		$$ = $1;
		$$->push_back($3);
		GEN_LOC(P($$), P($1), P($3)); }

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
