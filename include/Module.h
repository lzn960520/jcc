#ifndef _MODULE_H_
#define _MODULE_H_

#include <list>

#include "ASTNode.h"

class Namespace;
class Module : public ASTNode {
	Namespace *ns;
	std::list<ASTNode *> &list;
public:
	Module(Namespace *ns, std::list<ASTNode *> *definations);
	~Module();
	Json::Value json() override;
	void genStruct(Context &context);
	void gen(Context &context) override;
};

#endif
