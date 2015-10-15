#include <llvm/IR/IRBuilder.h>

#include "ArrayAccess.h"
#include "Context.h"
#include "exception.h"
#include "ArrayAccessor.h"
#include "Type.h"

ArrayAccess::ArrayAccess(Expression *array, ArrayAccessor *accessor) :
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

llvm::Value* ArrayAccess::load(Context &context) {
	const std::vector<std::pair<int, int> > &dim = array->getType(context)->arrayDim;
	llvm::Value *offset = NULL;
	for (size_t i = 0; i < dim.size(); i++) {
		if (dim[i].first >= dim[i].second) {
			// TODO: Dynamic array
			throw NotImplemented("Dynamic array");
		} else {
			if (offset == NULL)
				offset = context.getBuilder().CreateSub(
						accessor->list[i]->load(context),
						llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), dim[i].first, true));
			else {
				offset = context.getBuilder().CreateMul(
						offset,
						llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), dim[i].second - dim[i].first + 1, false));
				offset = context.getBuilder().CreateAdd(
						offset,
						context.getBuilder().CreateSub(
								accessor->list[i]->load(context),
								llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), dim[i].first, true)
						)
				);
			}
		}
	}
	offset = context.getBuilder().CreateMul(
			offset,
			llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), array->getType(context)->internal->getSize(), false));
	return context.getBuilder().CreateLoad(
			context.getBuilder().CreateIntToPtr(
					context.getBuilder().CreateAdd(
							context.getBuilder().CreatePointerCast(
									array->load(context),
									context.getBuilder().getInt32Ty()
							),
							offset),
					llvm::Type::getInt32PtrTy(context.getContext())
			)
	);
}

void ArrayAccess::store(Context &context, llvm::Value *value) {
	const std::vector<std::pair<int, int> > &dim = array->getType(context)->arrayDim;
	llvm::Value *offset = NULL;
	for (size_t i = 0; i < dim.size(); i++) {
		if (dim[i].first >= dim[i].second) {
			// TODO: Dynamic array
			throw NotImplemented("Dynamic array");
		} else {
			if (offset == NULL)
				offset = context.getBuilder().CreateSub(
						accessor->list[i]->load(context),
						llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), dim[i].first, false));
			else {
				offset = context.getBuilder().CreateMul(
						offset,
						llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), dim[i].second - dim[i].first + 1, false));
				offset = context.getBuilder().CreateAdd(
						offset,
						context.getBuilder().CreateSub(
								accessor->list[i]->load(context),
								llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), dim[i].first, true)
						)
				);
			}
		}
	}
	offset = context.getBuilder().CreateMul(
			offset,
			llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), array->getType(context)->internal->getSize(), false));
	context.getBuilder().CreateStore(
			value,
			context.getBuilder().CreateIntToPtr(
				context.getBuilder().CreateAdd(
						context.getBuilder().CreatePointerCast(
								array->load(context),
								context.getBuilder().getInt32Ty()
						),
						offset),
				llvm::Type::getInt32PtrTy(context.getContext())
			)
	);
}

Type* ArrayAccess::getType(Context &context) {
	return array->getType(context)->internal;
}

bool ArrayAccess::isConstant() {
	throw NotImplemented("const array");
}

Expression::Constant ArrayAccess::loadConstant() {
	throw NotImplemented("const array");
}

Type* ArrayAccess::getTypeConstant() {
	throw NotImplemented("const array");
}
