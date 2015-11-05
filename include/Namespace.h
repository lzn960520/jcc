#ifndef _NAMESPACE_H_
#define _NAMESPACE_H_

#include <list>

#include "ASTNode.h"

class Identifier;
class Namespace : public ASTNode {
	std::list<Identifier *> list;
public:
	Namespace(Identifier *identifier);
	void push_back(Identifier *identifier);
	~Namespace();
	Json::Value json() override;
	const std::string getFullName();
	const std::string getMangleName();
};

#endif
