#ifndef _YYVAL_TYPES_H_
#define _YYVAL_TYPES_H_

#include <string>
#include <list>
#include <json/json.h>
#include <llvm/IR/IRBuilder.h>
#include "context.h"

enum Visibility {
	PRIVATE,
	PUBLIC,
	PROTECTED
};
extern const char *VisibilityNames[];

struct ASTNode {
	virtual Json::Value json() = 0;
	virtual void* gen(Context &context) { return NULL; };
	virtual ~ASTNode() {};
};

struct Op2 : public ASTNode {
	ASTNode *left, *right;
	enum OpType {
		ADD,
		SUB,
		MUL,
		DIV,
		MOD,
		PWR
	} op;
	Op2(ASTNode *left, OpType op, ASTNode *right);
	static const char *OpNames(OpType op);
	~Op2();
	Json::Value json();
	void* gen(Context &context) override;
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
	void *gen(Context &context);
};

struct LiteralString : public ASTNode {
	std::string text;
	LiteralString(const char *text);
	~LiteralString();
	Json::Value json();
	void *gen(Context &context);
};

struct Identifier : public ASTNode {
	std::string text;
	Identifier(const char *name);
	~Identifier();
	Json::Value json();
	std::string getName();
	void* gen(Context &context);
};

struct Statements : public ASTNode {
	std::list<ASTNode*> statements;
	Statements(ASTNode *statement);
	void push_back(ASTNode *statement);
	~Statements();
	Json::Value json();
	void* gen(Context &context);
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
	llvm::Type *getType(Context &context);
};

struct VariableDefination : public ASTNode {
	Type *type;
	std::list<std::pair<Identifier*,ASTNode*> > list;
	VariableDefination(ASTNode *type);
	void push_back(ASTNode *identifier);
	void push_back(ASTNode *identifier, ASTNode *init_value);
	~VariableDefination();
	Json::Value json();
	void *gen(Context &context) override;
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

struct CallArgumentList : public ASTNode {
	std::list<ASTNode*> list;
	CallArgumentList();
	CallArgumentList(ASTNode *expression);
	void push_back(ASTNode *expression);
	~CallArgumentList();
	Json::Value json();
};

struct FunctionCall : public ASTNode {
	Identifier *identifier;
	CallArgumentList *arg_list;
	FunctionCall(ASTNode *identifier, ASTNode *arg_list);
	~FunctionCall();
	Json::Value json();
	void* gen(Context &context);
};

#endif
