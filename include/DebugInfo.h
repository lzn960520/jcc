#ifndef _DEBUG_INFO_H_
#define _DEBUG_INFO_H_

#include "yyvaltypes.h"
#include "Context.h"
#include <iostream>
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/DebugInfo.h>

class Context;
class Function;
class Type;
llvm::DIType* getDIType(Context &context, Type *type);
llvm::DISubroutineType* getDIType(Context &context, Function *function);
llvm::DISubprogram* getDIFunction(Context &context, Function *function, YYLTYPE &loc);
template<typename T>
T* addDebugLoc(Context &context, T *instruction, YYLTYPE &loc) {
	((llvm::Instruction *) instruction)->setMetadata("dbg", llvm::DILocation::get(context.getContext(), loc.first_line, loc.first_column, context.DIfunction));
	return instruction;
}
void insertDeclareDebugInfo(Context &context, Type *type, const std::string &name, llvm::Value *val, YYLTYPE &loc, bool isArg);

#endif
