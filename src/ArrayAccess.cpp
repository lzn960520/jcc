#include <llvm/IR/IRBuilder.h>

#include "ArrayAccess.h"
#include "Context.h"
#include "exception.h"
#include "ArrayAccessor.h"
#include "Type.h"
#include "DebugInfo.h"

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
				//offset = addDebugLoc(
				//		context,
				//		context.getBuilder().CreateSub(
				//				accessor->list[i]->load(context),
				//				llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), dim[i].first, true)),
				//		loc);
				offset = context.getBuilder().CreateSub(
								accessor->list[i]->load(context),
								llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), dim[i].first, true));
			else {
				offset = addDebugLoc(
						context,
						context.getBuilder().CreateMul(
								offset,
								llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), dim[i].second - dim[i].first + 1, false)),
						loc);
				offset = addDebugLoc(
						context,
						context.getBuilder().CreateAdd(
								offset,
								addDebugLoc(
										context,
										context.getBuilder().CreateSub(
												accessor->list[i]->load(context),
												llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), dim[i].first, true)),
										loc)),
						loc);
			}
		}
	}
	llvm::Value *index[2];
	index[0] = llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), 0, false);
	index[1] = offset;
	llvm::Value *array_ptr = array->load(context);
	return addDebugLoc(
			context,
			context.getBuilder().CreateLoad(
					addDebugLoc(
							context,
							llvm::GetElementPtrInst::Create(
									nullptr,
									array_ptr,
									llvm::ArrayRef<llvm::Value*>(index, 2),
									"",
									context.currentBlock()),
							loc)),
			loc);
}

llvm::Instruction* ArrayAccess::store(Context &context, llvm::Value *value) {
	const std::vector<std::pair<int, int> > &dim = array->getType(context)->arrayDim;
	llvm::Value *offset = NULL;
	for (size_t i = 0; i < dim.size(); i++) {
		if (dim[i].first >= dim[i].second) {
			// TODO: Dynamic array
			throw NotImplemented("Dynamic array");
		} else {
			if (offset == NULL)
				//offset = addDebugLoc(
				//		context,
				//		context.getBuilder().CreateSub(
				//				accessor->list[i]->load(context),
				//				llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), dim[i].first, false)),
				//		loc);
				offset = context.getBuilder().CreateSub(accessor->list[i]->load(context),
								llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), dim[i].first, false));
			else {
				offset = addDebugLoc(
						context,
						context.getBuilder().CreateMul(
								offset,
								llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), dim[i].second - dim[i].first + 1, false)),
						loc);
				offset = addDebugLoc(
						context,
						context.getBuilder().CreateAdd(
								offset,
								context.getBuilder().CreateSub(
										accessor->list[i]->load(context),
										llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), dim[i].first, true))),
						loc);
			}
		}
	}
	llvm::Value *index[2];
	index[0] = llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), 0, false);
	index[1] = offset;
	return context.getBuilder().CreateStore(
			value,
			addDebugLoc(
					context,
					llvm::GetElementPtrInst::Create(
							nullptr,
							array->load(context),
							llvm::ArrayRef<llvm::Value*>(index, 2),
							"",
							context.currentBlock()),
					loc));
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
