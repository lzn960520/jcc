#include "DebugInfo.h"
#include "Type.h"
#include "Context.h"
#include "Function.h"

using llvm::DIType;
using llvm::SmallVector;
using llvm::Value;
using llvm::DISubroutineType;
using llvm::DISubprogram;
using llvm::Metadata;
using namespace llvm::dwarf;

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
	}
}

DISubroutineType* getDIType(Context &context, Function *function) {
	SmallVector<Metadata*, 16> argv;
	argv.push_back(getDIType(context, function->getReturnType()));
	for (Function::arg_iterator it = function->arg_begin(); it != function->arg_end(); it++)
		argv.push_back(getDIType(context, it->first));
	return context.DI->createSubroutineType(context.DIfile, context.DI->getOrCreateTypeArray(argv));
}

DISubprogram* getDIFunction(Context &context, Function *function) {
	return context.DI->createFunction(
			context.DIfile,
			function->getName(),
			function->getMangleName(),
			context.DIfile,
			1,
			getDIType(context, function),
			false,
			function->body != NULL,
			1,
			0,
			false,
			function->llvmFunction != nullptr ? function->llvmFunction : nullptr);
}
