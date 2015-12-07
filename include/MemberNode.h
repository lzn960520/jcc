#ifndef _MEMBER_NODE_H_
#define _MEMBER_NODE_H_

#include "StructNode.h"

class Class;
class Interface;
class MemberNode : public StructNode {
	friend class Class;
	friend class Interface;
protected:
	Class *cls;
public:
	inline Class* getClass() const { return cls; }
};

#endif
