#include "DebugInfo.h"
#include "Type.h"
#include "Context.h"
#include "Function.h"
#include "util.h"

using llvm::DIType;
using llvm::SmallVector;
using llvm::Value;
using llvm::DISubroutineType;
using llvm::DISubprogram;
using llvm::Metadata;
using llvm::DILocalVariable;
using namespace llvm::dwarf;
using std::string;
using llvm::DILocation;
using llvm::DIFile;

DIType* getDIType(Context &context, Type *type) {
	if (!type)
		return NULL;
	switch (type->baseType) {
	case Type::INT:
		if (type->isUnsigned)
			return context.DI->createBasicType("unsigned int", 32, 32, DW_ATE_unsigned);
		else
			return context.DI->createBasicType("int", 32, 32, DW_ATE_signed);
	case Type::SHORT:
		if (type->isUnsigned)
			return context.DI->createBasicType("unsigned short", 16, 16, DW_ATE_unsigned);
		else
			return context.DI->createBasicType("short", 16, 16, DW_ATE_signed);
	case Type::BYTE:
		if (type->isUnsigned)
			return context.DI->createBasicType("unsigned byte", 8, 8, DW_ATE_unsigned);
		else
			return context.DI->createBasicType("byte", 8, 8, DW_ATE_signed);
	case Type::CHAR:
		return context.DI->createBasicType("char", 16, 16, DW_ATE_unsigned_char);
	case Type::BOOL:
		return context.DI->createBasicType("bool", 1, 8, DW_ATE_boolean);
	case Type::FLOAT:
		return context.DI->createBasicType("float", 32, 32, DW_ATE_float);
	case Type::DOUBLE:
		return context.DI->createBasicType("double", 64, 64, DW_ATE_float);
	case Type::STRING:
		return context.DI->createBasicType("string", context.DL->getPointerSizeInBits(0), context.DL->getPointerSizeInBits(0), DW_ATE_float);
	case Type::OBJECT:
		return context.DI->createPointerType(getDIType(context, (Type *) NULL), context.DL->getPointerSizeInBits(0), context.DL->getPointerSizeInBits(0));
	}
}

DISubroutineType* getDIType(Context &context, Function *function) {
	SmallVector<Metadata*, 16> argv;
	argv.push_back(getDIType(context, function->getReturnType()));
	for (Function::arg_iterator it = function->arg_begin(); it != function->arg_end(); it++)
		argv.push_back(getDIType(context, it->first));
	return context.DI->createSubroutineType(context.currentDIScope()->getFile(), context.DI->getOrCreateTypeArray(argv));
}

DIFile *getDIFile(Context &context, const std::string path) {
	return llvm::DIFile::get(context.getContext(), getFilename(path), getDir(path));
}

DIFile *getDIFile(Context &context, Location &loc) {
	return getDIFile(context, loc.begin.filename);
}

DISubprogram* getDIFunction(Context &context, Function *function, Location &loc) {
	return context.DI->createFunction(
			getDIFile(context, loc),
			function->getName(),
			function->getMangleName(),
			getDIFile(context, loc),
			loc.begin.line,
			getDIType(context, function),
			false,
			!function->isDeclaration(),
			loc.begin.line,
			0,
			false,
			function->llvmFunction != nullptr ? function->llvmFunction : nullptr);
}

void insertDeclareDebugInfo(Context &context, Type *type, const string &name, llvm::Value *val, Location &loc, bool isArg) {
	addDebugLoc(
			context,
			context.DI->insertDeclare(
					val,
					context.DI->createLocalVariable(
							isArg ? DW_TAG_arg_variable : DW_TAG_auto_variable,
							context.currentDIScope(),
							name,
							context.currentDIScope()->getFile(),
							loc.begin.line,
							getDIType(context, type)),
					context.DI->createExpression(llvm::ArrayRef<uint64_t>(NULL, (uint32_t) 0)), 
					DILocation::get(context.getContext(), loc.begin.line, loc.begin.column, context.currentDIScope()),
					context.currentBlock()),
			loc);
}
