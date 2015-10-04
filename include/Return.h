#ifndef _RETURN_H_
#define _RETURN_H_

#include "ASTNode.h"

struct Return : public ASTNode {
	ASTNode *expression;
	Return();
	Return(ASTNode *expression);
	~Return();
	Json::Value json() override;
	void* gen(Context &context) override;
};

#endif
