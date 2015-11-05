#ifndef _MODULE_H_
#define _MODULE_H_

#include <list>

#include "StructNode.h"

class Namespace;
class Module : public StructNode {
	Namespace *ns;
	std::list<StructNode *> &list;
	Module *parent;
public:
	Module(Namespace *ns, std::list<StructNode *> *definations);
	~Module();
	Json::Value json() override;
	void genStruct(Context &context) override;
	void gen(Context &context) override;
	const std::string getFullName();
	const std::string getMangleName();
	void writeJsymFile(FILE *f) override;
};

#endif
