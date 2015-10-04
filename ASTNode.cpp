#include "yyvaltypes.h"
#include "jascal.tab.hpp"

ASTNode::ASTNode() {
	loc = yylloc;
}
