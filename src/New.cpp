#include <llvm/IR/DataLayout.h>

#include "New.h"
#include "Type.h"
#include "Context.h"
#include "exception.h"
#include "DebugInfo.h"
#include "Class.h"
#include "ArrayAccessor.h"
#include "LiteralInt.h"
#include "ArrayDefinator.h"

New::New(Type *type) : type(type), dim(NULL) {
}

New::New(Type *type, ArrayAccessor *dim) : type(NULL), dim(dim) {
	ArrayDefinator *definator = NULL;
	for (std::vector<Expression*>::const_iterator it = dim->list.begin(); it != dim->list.end(); it ++)
		if (definator == NULL)
			definator = new ArrayDefinator(new LiteralInt(0, false), *it);
		else
			definator->push_back(new LiteralInt(0, false), *it);
	this->type = new Type(type, definator);
}

New::~New() {
	delete type;
	if (dim)
		delete dim;
}

Json::Value New::json() {
	Json::Value root;
	root["name"] = "new";
	root["type"] = type->json();
	if (dim)
		root["dim"] = dim->json();
	return root;
}

llvm::Value *New::load(Context &context) {
	if (dim) {
		std::vector<llvm::Value*> dims;
		llvm::Value *arg[1] = {};
		for (std::vector<Expression*>::const_reverse_iterator it = dim->list.rbegin(); it != dim->list.rend(); it++) {
			dims.push_back((*it)->load(context));
			if (arg[0])
				arg[0] = addDebugLoc(
						context,
						context.getBuilder().CreateMul(
								arg[0],
								dims.back()
						),
						loc
				);
			else
				arg[0] = dims.back();
		}
		arg[0] = context.getBuilder().CreateIntCast(
				context.getBuilder().CreateAdd(
						context.getBuilder().CreateMul(
								arg[0],
								context.getBuilder().getInt32(type->internal->getSize(context))
						),
						context.getBuilder().getInt32(dims.size() * context.DL->getTypeAllocSize(context.getBuilder().getInt32Ty()))
				),
				context.getBuilder().getInt64Ty(),
				false
		);
		llvm::Value *newObj = addDebugLoc(
				context,
				context.getBuilder().CreateBitCast(
						addDebugLoc(
								context,
								llvm::CallInst::Create(
										context.mallocFunc,
										llvm::ArrayRef<llvm::Value*>(arg, 1),
										"",
										context.currentBlock()
								),
								loc),
						llvm::PointerType::get(context.getBuilder().getInt32Ty(), 0)
				),
				loc);
		{
			// save size data
			size_t i = 0;
			for (std::vector<llvm::Value*>::iterator it = dims.begin(); it != dims.end(); it++, i++)
				addDebugLoc(
						context,
						context.getBuilder().CreateStore(
								*it,
								addDebugLoc(
										context,
										context.getBuilder().CreateConstGEP1_32(
												newObj,
												i
										),
										loc
								)
						),
						loc
				);
		}
		return addDebugLoc(
				context,
				context.getBuilder().CreatePointerCast(
						addDebugLoc(
								context,
								context.getBuilder().CreateConstGEP1_32(
										newObj,
										dims.size()
								),
								loc
						),
						llvm::PointerType::get(type->internal->getType(context), 0)
				),
				loc
		);
	} else {
		llvm::Value *arg[] = { context.getBuilder().getInt64(type->getSize(context)) };
		llvm::Value *newObj = addDebugLoc(
				context,
				context.getBuilder().CreateBitCast(
						addDebugLoc(
								context,
								llvm::CallInst::Create(
										context.mallocFunc,
										llvm::ArrayRef<llvm::Value*>(arg, 1),
										"",
										context.currentBlock()
								),
								loc),
						type->getType(context)),
				loc);
		if (type->isObject()) {
			Class *cls = type->getClass();
			std::vector<llvm::Value*> arg;
			arg.push_back(newObj);
			addDebugLoc(
					context,
					context.getBuilder().CreateCall(cls->getConstructor(), llvm::ArrayRef<llvm::Value*>(arg)),
					loc
			);
		}
		return newObj;
	}
}

llvm::Instruction* New::store(Context &context, llvm::Value *value) {
	throw NotAssignable("object create");
}

Type* New::getType(Context &context) {
	return type;
}

Expression::Constant New::loadConstant() {
	throw NotConstant("object create");
}

Type* New::getTypeConstant() {
	throw NotConstant("object create");
}

New* New::clone() const {
	if (dim)
		return new New(type->clone(), dim->clone());
	else
		return new New(type->clone(), NULL);
}
