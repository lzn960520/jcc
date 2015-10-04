#ifndef _ASTNODE_H_
#define _ASTNODE_H_

#include <json/json.h>
#include "yyvaltypes.h"

class Context;

struct ASTNode {
	YYLTYPE loc;
	ASTNode();
	virtual Json::Value json() = 0;
	virtual void* gen(Context &context) { return NULL; };
	virtual ~ASTNode() {};
};

#endif
