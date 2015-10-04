#ifndef _YYVAL_TYPES_H_
#define _YYVAL_TYPES_H_

#include <string>
#include <list>
#include <json/json.h>
#include <llvm/IR/IRBuilder.h>
#include "context.h"

struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
#define YYLTYPE_IS_DECLARED

enum Visibility {
	PRIVATE,
	PUBLIC,
	PROTECTED
};
extern const char *VisibilityNames[];

struct ASTNode {
	YYLTYPE loc;
	ASTNode();
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
	Json::Value json() override;
	void* gen(Context &context) override;
};

struct CompileUnit : public ASTNode {
	ASTNode *body;
	CompileUnit(ASTNode *body);
	~CompileUnit();
	Json::Value json() override;
};

struct LiteralInt : public ASTNode {
	int val;
	LiteralInt(int val);
	~LiteralInt();
	Json::Value json() override;
	void *gen(Context &context) override;
};

struct LiteralString : public ASTNode {
	std::string text;
	LiteralString(const char *text);
	~LiteralString();
	Json::Value json() override;
	void *gen(Context &context) override;
};

struct Identifier : public ASTNode {
	std::string text;
	Identifier(const char *name);
	~Identifier();
	Json::Value json() override;
	const std::string& getName() const;
	void* gen(Context &context) override;
};

struct Statements : public ASTNode {
	std::list<ASTNode*> statements;
	Statements(ASTNode *statement);
	void push_back(ASTNode *statement);
	~Statements();
	Json::Value json() override;
	void* gen(Context &context) override;
};

struct IfStatement : public ASTNode {
	ASTNode *test, *then_st, *else_st;
	IfStatement(ASTNode *test, ASTNode *then_st);
	IfStatement(ASTNode *test, ASTNode *then_st, ASTNode *else_st);
	~IfStatement();
	Json::Value json() override;
};

struct WhileStatement : public ASTNode {
	ASTNode *test, *body;
	WhileStatement(ASTNode *test, ASTNode *body);
	~WhileStatement();
	Json::Value json() override;
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
	Json::Value json() override;
	llvm::Type *getType(Context &context);
};

struct VariableDefination : public ASTNode {
	Type *type;
	std::list<std::pair<Identifier*,ASTNode*> > list;
	VariableDefination(ASTNode *type);
	void push_back(ASTNode *identifier);
	void push_back(ASTNode *identifier, ASTNode *init_value);
	~VariableDefination();
	Json::Value json() override;
	void *gen(Context &context) override;
};

struct ArgumentList : public ASTNode {
	typedef std::list<std::pair<Type*, Identifier*> > ListType;
	ListType list;
	ArgumentList();
	ArgumentList(Type *type, Identifier *identifier);
	void push_back(Type *type, Identifier *identifier);
	~ArgumentList();
	Json::Value json() override;
};

struct Function : public ASTNode {
	Visibility visibility;
	Type *return_type;
	Identifier *identifier;
	ArgumentList *arg_list;
	ASTNode *body;
	Function(Visibility visibility, Type *return_type, Identifier *identifier, ArgumentList *arg_list, ASTNode *body);
	~Function();
	Json::Value json() override;
	void *gen(Context &context) override;
};

struct CallArgumentList : public ASTNode {
	std::list<ASTNode*> list;
	CallArgumentList();
	CallArgumentList(ASTNode *expression);
	void push_back(ASTNode *expression);
	~CallArgumentList();
	Json::Value json() override;
};

struct FunctionCall : public ASTNode {
	Identifier *identifier;
	CallArgumentList *arg_list;
	FunctionCall(ASTNode *identifier, ASTNode *arg_list);
	~FunctionCall();
	Json::Value json() override;
	void* gen(Context &context) override;
};

struct Block : public ASTNode {
	Statements *body;
	Block(Statements *body);
	~Block();
	Json::Value json() override;
	void* gen(Context &context) override;
};

struct Return : public ASTNode {
	ASTNode *expression;
	Return();
	Return(ASTNode *expression);
	~Return();
	Json::Value json() override;
	void* gen(Context &context) override;
};

#endif
