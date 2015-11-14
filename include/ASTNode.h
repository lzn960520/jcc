#ifndef _ASTNODE_H_
#define _ASTNODE_H_

#include <json/json.h>

#include "location.h"

class Context;
struct ASTNode {
	Location loc;
	ASTNode();
	virtual Json::Value json() = 0;
	virtual void gen(Context &context) {};
	virtual ~ASTNode() {};
};

#endif
