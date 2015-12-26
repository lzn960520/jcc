#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>

#include "ArrayAccess.h"
#include "Context.h"
#include "exception.h"
#include "ArrayAccessor.h"
#include "Type.h"
#include "DebugInfo.h"

ArrayAccess::ArrayAccess(Expression *array, ArrayAccessor *accessor) :
	array(array), accessor(accessor) {
}

ArrayAccess* ArrayAccess::clone() const {
	return new ArrayAccess(array->clone(), accessor->clone());
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

llvm::Value* ArrayAccess::getPointer(Context &context) {
	const std::vector<std::pair<Expression*, Expression*> > &dim = array->getType(context)->arrayDim;
	llvm::Value *offset = NULL;
	for (size_t i = 0; i < dim.size(); i++) {
		if (!dim[i].first->getTypeConstant()->isInt() || (dim[i].second && !dim[i].second->getType(context)->isInt()))
			throw InvalidType("dim expression must be integer");
		else {
			if (offset == NULL)
				offset = addDebugLoc(
						context,
						context.getBuilder().CreateSub(
								accessor->list[i]->load(context),
								context.getBuilder().getInt32(dim[i].first->loadConstant()._int64)
						),
						loc);
			else {
				if (!dim[i - 1].second || !dim[i - 1].second->isConstant())
					offset = addDebugLoc(
							context,
							context.getBuilder().CreateMul(
									offset,
									context.getBuilder().CreateLoad(
											context.getBuilder().CreateConstGEP1_32(
													context.getBuilder().CreatePointerCast(
															offset,
															llvm::PointerType::get(context.getBuilder().getInt32Ty(), 0)
													),
													-(i + 1)
											)
									)
							),
							loc);
				else
					offset = addDebugLoc(
							context,
							context.getBuilder().CreateMul(
									offset,
									context.getBuilder().getInt32(dim[i].second->loadConstant()._int64 - dim[i].first->loadConstant()._int64 + 1)),
							loc);
				offset = addDebugLoc(
						context,
						context.getBuilder().CreateAdd(
								offset,
								addDebugLoc(
										context,
										llvm::BinaryOperator::CreateSub(
												accessor->list[i]->load(context),
												context.getBuilder().getInt32(dim[i].first->loadConstant()._int64),
												"",
												context.currentBlock()),
										loc)),
						loc);
			}
		}
	}
	llvm::Value *index[1];
	index[0] = offset;
	llvm::Value *array_ptr = array->load(context);
	return addDebugLoc(
			context,
			context.getBuilder().CreateInBoundsGEP(
					array_ptr,
					llvm::ArrayRef<llvm::Value*>(index, 1)
			),
			loc);
}

llvm::Value* ArrayAccess::load(Context &context) {
	return addDebugLoc(
			context,
			context.getBuilder().CreateLoad(
					getPointer(context)),
			loc);
}

llvm::Instruction* ArrayAccess::store(Context &context, llvm::Value *value) {
	return context.getBuilder().CreateStore(
			value,
			getPointer(context));
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
