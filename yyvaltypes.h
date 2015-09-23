#ifndef _YYVAL_TYPES_H_
#define _YYVAL_TYPES_H_

#include <string>
#include <list>
#include <json/json.h>
#include <llvm/IR/IRBuilder.h>

enum Visibility {
	PRIVATE,
	PUBLIC,
	PROTECTED
};
extern const char *VisibilityNames[];

struct ASTNode {
	virtual Json::Value json() = 0;
	virtual void gen() {};
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
	void gen(llvm::IRBuilder<> &builder);
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

struct WhileStatement : public ASTNode {
	ASTNode *test, *body;
	WhileStatement(ASTNode *test, ASTNode *body);
	~WhileStatement();
	Json::Value json();
};

struct VariableDefination : public ASTNode {
	ASTNode *type;
	std::list<std::pair<ASTNode*,ASTNode*> > list;
	VariableDefination(ASTNode *type);
	void push_back(ASTNode *identifier);
	void push_back(ASTNode *identifier, ASTNode *init_value);
	~VariableDefination();
	Json::Value json();	
};

struct Type : public ASTNode {
	enum BaseType {
		BYTE,
		SHORT,
		INT,
		CHAR,
		FLOAT,
		DOUBLE
	} baseType;
	static const char *BaseTypeNames[];
	bool isUnsigned;
	Type(BaseType baseType, bool isUnsigned = false);
	~Type();
	Json::Value json();
};

struct Function : public ASTNode {
	Visibility visibility;
	ASTNode *return_type, *identifier, *arg_list, *body;
	Function(Visibility visibility, ASTNode *return_type, ASTNode *identifier, ASTNode *arg_list, ASTNode *body);
	~Function();
	Json::Value json();
};

struct ArgumentList : public ASTNode {
	std::list<std::pair<ASTNode*,ASTNode*> > list;
	ArgumentList();
	ArgumentList(ASTNode *type, ASTNode *identifier);
	void push_back(ASTNode *type, ASTNode *identifier);
	~ArgumentList();
	Json::Value json();
};

#endif
