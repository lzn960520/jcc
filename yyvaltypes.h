#ifndef _YYVAL_TYPES_H_
#define _YYVAL_TYPES_H_

#include <string>
#include <list>
#include <jsoncpp/json/json.h>

struct ASTNode {
	virtual Json::Value json() = 0;
	virtual ~ASTNode() {};
};

struct Expression : public ASTNode {
	enum Type {
		OP2,
		OP1,
		LITERAL,
		ENCLOSED,
		IDENTIFIER,
		FUNCTION_CALL,
		NEW,
		DELETE
	} type;
	static const char *TypeNames[];
	Expression(ASTNode *single, Type type);
	ASTNode *left, *right, *args;
	enum OpType {
		ADD,
		SUB,
		MUL,
		DIV,
		MOD,
		PWR
	} op;
	Expression(ASTNode *left, OpType op, ASTNode *right);
	static const char *OpNames(OpType op);
	int assignable:1;
	int val;
	~Expression();
	Json::Value json();
};

struct CompileUnit : public ASTNode {
	ASTNode *body;
	CompileUnit(ASTNode *body);
	~CompileUnit();
	Json::Value json();
};

struct LiteralInt : public ASTNode {
	int val;
	LiteralInt(int val);
	~LiteralInt();
	Json::Value json();
};

struct LiteralString : public ASTNode {
	std::string text;
	LiteralString(const char *text);
	~LiteralString();
	Json::Value json();
};

struct Identifier : public ASTNode {
	std::string text;
	Identifier(const char *name);
	~Identifier();
	Json::Value json();
};

struct Statements : public ASTNode {
	std::list<ASTNode*> statements;
	Statements(ASTNode *statement);
	void push_back(ASTNode *statement);
	~Statements();
	Json::Value json();
};

struct IfStatement : public ASTNode {
	ASTNode *test, *then_st, *else_st;
	IfStatement(ASTNode *test, ASTNode *then_st);
	IfStatement(ASTNode *test, ASTNode *then_st, ASTNode *else_st);
	~IfStatement();
	Json::Value json();
};

#endif
