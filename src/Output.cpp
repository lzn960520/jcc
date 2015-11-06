#include <llvm/MC/MCContext.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/Analysis/Passes.h>

#include "Output.h"
#include "exception.h"
#include "util.h"

using namespace llvm;
using namespace std;
using legacy::PassManager;

class raw_pwrite_os_stream : public raw_pwrite_stream {
	ostream &os;
public:
	raw_pwrite_os_stream(ostream &os) : os(os) {}
	void write_impl(const char *p, size_t size) {
		os.write(p, size);
	}
	uint64_t current_pos() const {
		return os.tellp();
	}
	void pwrite_impl(const char *p, size_t size, uint64_t offset) {
		uint64_t oldp = os.tellp();
		os.seekp(offset);
		os.write(p, size);
		os.seekp(oldp);
	}
};

static void addOptPasses(PassManager &pm) {
	/*passManager.add(llvm::createConstantMergePass());
	passManager.add(llvm::createConstantPropagationPass());
	passManager.add(llvm::createLoopSimplifyPass());
	passManager.add(llvm::createCFGSimplificationPass());
	passManager.add(llvm::createInstructionSimplifierPass());
	passManager.add(llvm::createDeadInstEliminationPass());
	passManager.add(llvm::createDeadCodeEliminationPass());
	passManager.add(llvm::createUnreachableBlockEliminationPass());
	passManager.add(llvm::createGlobalDCEPass());*/
}

void outputLLVM(Module &module, ostream &os) {
	raw_pwrite_os_stream os_wrap(os);
	outputLLVM(module, os_wrap);
}

void outputLLVM(llvm::Module &module, raw_pwrite_stream &os) {
	PassManager pm;
	addOptPasses(pm);
	pm.add(createPrintModulePass(os));
	pm.run(module);
	os.flush();
}

void outputAssemble(Module &module, ostream &os) {
	raw_pwrite_os_stream os_wrap(os);
	outputAssemble(module, os_wrap);
}

void outputAssemble(llvm::Module &module, raw_pwrite_stream &os) {
	InitializeAllTargets();
	InitializeAllTargetMCs();
	InitializeAllAsmPrinters();
	string err;
	Triple triple;
	const Target *target = TargetRegistry::lookupTarget("x86", triple, err);
	TargetOptions opt;
	TargetMachine *tm = target->createTargetMachine(triple.getTriple(), "i386", "", opt);
	PassManager passManager;
	addOptPasses(passManager);

	tm->addPassesToEmitFile(passManager, os, TargetMachine::CGFT_AssemblyFile, false);
	passManager.run(module);
	os.flush();
}

#define TOKEN_PLACEHOLDER "HAHAHA_JASCAL_PLACEHOLDER_TOKEN"
#define AST_PLACEHOLDER "HAHAHA_JASCAL_PLACEHOLDER_AST"
void outputHtml(std::string lex, const Json::Value &ast, std::ostream &os) {
	extern const char htmlTemplate[];
	const char *pos = strstr(htmlTemplate, AST_PLACEHOLDER), *p = htmlTemplate;
	if (pos == NULL)
		throw CompileException("Wrong template format");
	for (; p < pos; p++)
		os << *p;
	Json::FastWriter writer;
	os << writer.write(ast);
	p += strlen(AST_PLACEHOLDER);

	pos = strstr(p, TOKEN_PLACEHOLDER);
	if (pos == NULL)
		throw CompileException("Wrong template foramt");
	for (; p < pos; p++)
		os << *p;
	replace_all_inplace(lex, "<", "&lt;");
	replace_all_inplace(lex, ">", "&gt;");
	os << lex;
	p += strlen(TOKEN_PLACEHOLDER);
	os << p;
}
