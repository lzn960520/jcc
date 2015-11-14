#ifndef _STRUCT_NODE_H_
#define _STRUCT_NODE_H_

#include "ASTNode.h"
#include <ostream>

class Context;
class StructNode : public ASTNode {
public:
	virtual void genStruct(Context &context) = 0;
	virtual void writeJsymFile(std::ostream &os) = 0;
};

#endif
