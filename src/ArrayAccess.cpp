#include <llvm/IR/IRBuilder.h>

#include "ArrayAccess.h"
#include "Context.h"
#include "exception.h"

ArrayAccess::ArrayAccess(ASTNode *array, ASTNode *accessor) :
	array(array), accessor(accessor) {
}

Json::Value ArrayAccess::json() {
	Json::Value root;
	root["name"] = "array_access";
	root["array"] = array->json();
	root["accessor"] = accessor->json();
	return root;
}

ArrayAccess::~ArrayAccess() {
	if (array)
		delete array;
	if (accessor)
		delete accessor;
}

void* ArrayAccess::gen(Context &context) {
	return NULL;
}
