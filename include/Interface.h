#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include <list>
#include <llvm/IR/DerivedTypes.h>

#include "Class.h"

class Identifier;
class MemberNode;
class Interface : public Class {
public:
	Interface(Identifier *identifier, std::list<MemberNode*> *list);
	~Interface();
	Json::Value json() override;
	void gen(Context &context) override;
	void genStruct(Context &context) override;
	void writeJsymFile(std::ostream &os) override;
	const std::string getMangleName() override;
};

#endif
