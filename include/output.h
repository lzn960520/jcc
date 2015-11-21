#ifndef _OUTPUT_H_
#define _OUTPUT_H_

#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>
#include <iosfwd>
#include <string>
#include <json/json.h>
#include <map>

void outputLLVM(llvm::Module &module, std::ostream &os);
void outputAssemble(llvm::Module &module, std::ostream &os);
void outputLLVM(llvm::Module &module, llvm::raw_pwrite_stream &os);
void outputAssemble(llvm::Module &module, llvm::raw_pwrite_stream &os);
void outputHtml(const std::map<std::string, std::string> &lex, const std::map<std::string, Json::Value> &ast, std::ostream &os);

#endif
