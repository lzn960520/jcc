#ifndef _OUTPUT_H_
#define _OUTPUT_H_

#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>
#include <iosfwd>

void outputLLVM(llvm::Module &module, std::ostream &os);
void outputAssemble(llvm::Module &module, std::ostream &os);
void outputLLVM(llvm::Module &module, llvm::raw_pwrite_stream &os);
void outputAssemble(llvm::Module &module, llvm::raw_pwrite_stream &os);

#endif
