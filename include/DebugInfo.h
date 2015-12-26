#ifndef _DEBUG_INFO_H_
#define _DEBUG_INFO_H_

#include <iostream>
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/DebugInfo.h>

#include "Context.h"
#include "location.h"

class Context;
class Function;
class Type;
llvm::DIType* getDIType(Context &context, Type *type);
llvm::DISubroutineType* getDIType(Context &context, Function *function);
llvm::DISubprogram* getDIFunction(Context &context, Function *function, Location &loc);
template<typename T>
T* addDebugLoc(Context &context, T *instruction, Location &loc) {
	if (llvm::isa<llvm::Constant>(instruction))
		return instruction;
	((llvm::Instruction *) instruction)->setMetadata("dbg", llvm::DILocation::get(context.getContext(), loc.begin.line, loc.begin.column, context.DIfunction));
	return instruction;
}
void insertDeclareDebugInfo(Context &context, Type *type, const std::string &name, llvm::Value *val, Location &loc, bool isArg);

#endif
