#ifndef _RETURN_H_
#define _RETURN_H_

#include "ASTNode.h"

class Expression;
struct Return : public ASTNode {
	Expression *expression;
	Return();
	Return(Expression *expression);
	~Return();
	Json::Value json() override;
	void gen(Context &context) override;
};

#endif
