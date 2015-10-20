#ifndef _DEBUG_INFO_H_
#define _DEBUG_INFO_H_

#include <llvm/IR/DIBuilder.h>

class Context;
class Function;
class Type;
llvm::DIType* getDIType(Context &context, Type *type);
llvm::DISubroutineType* getDIType(Context &context, Function *function);
llvm::DISubprogram* getDIFunction(Context &context, Function *function);

#endif
