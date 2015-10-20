#include "ASTNode.h"
#include "jascal.tab.hpp"

ASTNode::ASTNode() {
	loc = yylloc;
}
