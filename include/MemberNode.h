#ifndef _MEMBER_NODE_H_
#define _MEMBER_NODE_H_

#include "StructNode.h"

class Class;
class MemberNode : public StructNode {
	friend class Class;
protected:
	Class *cls;
public:
	inline Class* getClass() const { return cls; }
};

#endif
