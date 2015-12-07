#ifndef _STRUCT_NODE_H_
#define _STRUCT_NODE_H_

#include "ASTNode.h"
#include <ostream>

class Context;
class Module;
class StructNode : public ASTNode {
protected:
	Module *module;
	friend class Module;
public:
	virtual void genStruct(Context &context) = 0;
	virtual void writeJsymFile(std::ostream &os) = 0;
};

#endif
