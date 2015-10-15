#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fstream>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/PassManager.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/LinkAllPasses.h>
#include "context.h"
#include "exception.h"
#include <wait.h>
#include "cmdline.h"
#include "ASTNode.h"

// global
std::string input_filename;

// from flex
extern FILE *yyin;
extern FILE *lex_output;
extern void lex_only();

// from bison
extern void yyparse();
extern ASTNode *root;

// for cmdline process
#define OPT_LEX_ONLY 256
#define OPT_PARSE_ONLY 257
#define OPT_INPUT_FILE 258
#define OPT_DUMP_LEX 259
#define DEFAULT_LEX_OUTPUT "lex.txt"
#define OPT_DUMP_AST 260
#define DEFAULT_AST_OUTPUT "ast.json"
#define OPT_OUTPUT_FILE 261
#define OPT_COMPILE_ONLY 262
#define OPT_AS_ONLY 263
#define OPT_LLVM_ONLY 264

int main(int argc, char * const argv[]) {
	// process cmdline
	CmdLine cmdline(argc, argv);
	cmdline.registerOpt(OPT_DUMP_AST, "--dump-ast", CmdLine::OPT_ARG);
	cmdline.registerOpt(OPT_DUMP_LEX, "--dump-lex", CmdLine::OPT_ARG);
	cmdline.registerOpt(OPT_LEX_ONLY, "--lex", CmdLine::NO_ARG);
	cmdline.registerOpt(OPT_PARSE_ONLY, "--parse", CmdLine::NO_ARG);
	cmdline.registerOpt(OPT_OUTPUT_FILE, "-o", CmdLine::REQUIRE_ARG);
	cmdline.registerOpt(OPT_COMPILE_ONLY, "-S", CmdLine::NO_ARG);
	cmdline.registerOpt(OPT_AS_ONLY, "-c", CmdLine::NO_ARG);
	cmdline.registerOpt(OPT_LLVM_ONLY, "--llvm", CmdLine::NO_ARG);

	bool opt_lex_only = false, opt_parse_only = false,
				opt_dump_lex = false, opt_dump_ast = false,
				opt_as_only = false, opt_compile_only = false, opt_llvm_only = false;
	std::string ast_filename, output_filename;
	for (CmdLine::Option opt; cmdline.next(opt) != -2;) {
		switch (opt.id) {
		case OPT_LEX_ONLY:
			opt_lex_only = true;
			break;
		case OPT_PARSE_ONLY:
			opt_parse_only = true;
			break;
		case OPT_LLVM_ONLY:
			opt_llvm_only = true;
			break;
		case OPT_COMPILE_ONLY:
			opt_compile_only = true;
			break;
		case OPT_AS_ONLY:
			opt_as_only = true;
			break;
		case -1:
			if (yyin != NULL) {
				fprintf(stderr, "Can't specify two input files at the same time\n");
				exit(0);
			}
			yyin = fopen(opt.arg.c_str(), "r");
			if (yyin == NULL) {
				fprintf(stderr, "Can't open input file \"%s\", errno %d\n", optarg, errno);
				exit(0);
			}
			input_filename = opt.arg.c_str();
			break;
		case OPT_OUTPUT_FILE:
			if (!output_filename.empty()) {
				fprintf(stderr, "Can't specify two output files at the same time\n");
				exit(0);
			}
			output_filename = opt.arg;
			break;
		case OPT_DUMP_LEX:
			opt_dump_lex = true;
			if (lex_output != NULL) {
				fprintf(stderr, "Can't specify two lex output file\n");
				exit(0);
			}
			if (opt.arg.empty())
				lex_output = fopen(DEFAULT_LEX_OUTPUT, "w");
			else
				lex_output = fopen(opt.arg.c_str(), "w");
			if (lex_output == NULL) {
				fprintf(stderr, "Can't open lex output file \"%s\"\n", opt.arg.empty() ? DEFAULT_LEX_OUTPUT : opt.arg.c_str());
				exit(0);
			}
			break;
		case OPT_DUMP_AST:
			opt_dump_ast = true;
			if (ast_filename != "") {
				fprintf(stderr, "Can't specify two ast output file\n");
				exit(0);
			}
			ast_filename = opt.arg.empty() ? DEFAULT_AST_OUTPUT : opt.arg.c_str();
			break;
		default:
			fprintf(stderr, "Unknown option %s\n", opt.opt.c_str());
			exit(0);
			break;
		}
	}

	if (opt_lex_only + opt_parse_only + opt_llvm_only + opt_as_only + opt_compile_only > 1) {
		fprintf(stderr, "You can't specify two --xxx-only options at the same time\n");
		exit(0);
	}

	do {
		try {
			if (opt_lex_only) {
				lex_only();
				break;
			}
			yyparse();
			if (opt_parse_only)
				break;
			if (!root)
				throw CompileException("No compile unit found");

			// call gen to generate llvm code
			Context *context = new Context();
			root->gen(*context);

			// optimization
			llvm::PassManager passManager;
			/*passManager.add(llvm::createConstantMergePass());
			passManager.add(llvm::createConstantPropagationPass());
			passManager.add(llvm::createLoopSimplifyPass());
			passManager.add(llvm::createCFGSimplificationPass());
			passManager.add(llvm::createInstructionSimplifierPass());
			passManager.add(llvm::createDeadInstEliminationPass());
			passManager.add(llvm::createDeadCodeEliminationPass());
			passManager.add(llvm::createUnreachableBlockEliminationPass());
			passManager.add(llvm::createGlobalDCEPass());*/

			if (opt_llvm_only) {
				if (output_filename.empty() && input_filename.empty()) {
					llvm::raw_fd_ostream ofs(1, false);
					passManager.add(llvm::createPrintModulePass(ofs));
					passManager.run(context->getModule());
					ofs.close();
				} else if (output_filename.empty()) {
					std::string oname = input_filename.substr(0, input_filename.rfind('.')) + ".llvm";
					std::ofstream sys_ofs(oname);
					llvm::raw_os_ostream ofs(sys_ofs);
					passManager.add(llvm::createPrintModulePass(ofs));
					passManager.run(context->getModule());
					ofs.flush();
					sys_ofs.close();
				} else {
					std::ofstream sys_ofs(output_filename);
					llvm::raw_os_ostream ofs(sys_ofs);
					passManager.add(llvm::createPrintModulePass(ofs));
					passManager.run(context->getModule());
					ofs.flush();
					sys_ofs.close();
				}
				break;
			}

			// write out
			int pipe_me_llvm_as[2], pipe_llvm_as_llc[2];
			pipe(pipe_me_llvm_as);
			pipe(pipe_llvm_as_llc);
			int pid;
			if ((pid = fork()) < 0)
				throw CompileException("Can't fork");
			else if (pid == 0) {
				dup2(pipe_me_llvm_as[0], 0);
				dup2(pipe_llvm_as_llc[1], 1);
				close(pipe_me_llvm_as[0]);
				close(pipe_me_llvm_as[1]);
				close(pipe_llvm_as_llc[0]);
				close(pipe_llvm_as_llc[1]);
				execlp("llvm-as", "llvm-as", "-", NULL);
			}
			if ((pid = fork()) < 0)
				throw CompileException("Can't fork");
			else if (pid == 0) {
				dup2(pipe_llvm_as_llc[0], 0);
				close(pipe_me_llvm_as[0]);
				close(pipe_me_llvm_as[1]);
				close(pipe_llvm_as_llc[0]);
				close(pipe_llvm_as_llc[1]);
				if (opt_compile_only)
					execlp("llc", "llc", "-filetype=asm" , "-o", output_filename.c_str(), "-", NULL);
				else if (opt_as_only)
					execlp("llc", "llc", "-filetype=obj", "-o", output_filename.c_str(), "-", NULL);
				else
					execlp("llc", "llc", "-filetype=obj", "-o", "tmp.o", "-", NULL);
			}
			close(pipe_me_llvm_as[0]);
			close(pipe_llvm_as_llc[0]);
			close(pipe_llvm_as_llc[1]);
			{
				llvm::raw_fd_ostream ofs(pipe_me_llvm_as[1], false);
				passManager.add(llvm::createPrintModulePass(ofs));
				passManager.run(context->getModule());
				ofs.flush();
				ofs.close();
			}

			// wait for llc terminate
			waitpid(pid, NULL, 0);

			if (opt_as_only || opt_compile_only)
				break;

			// exec gcc to build executable
			std::string cmdline = "gcc";
			if (!output_filename.empty()) {
				cmdline += " -o ";
				cmdline += output_filename;
			}
			cmdline += " tmp.o";
			system(cmdline.c_str());

			unlink("tmp.o");
		} catch (CompileException &e) {
			fprintf(stderr, "Compile error: %s\n", e.message().c_str());
		}
	} while(0);

	if (opt_dump_lex)
		fclose(lex_output);
	if (opt_dump_ast && root) {
		Json::Value json_root = root->json();
		std::ofstream ofs(ast_filename);
		if (!ofs) {
			fprintf(stderr, "Can't open ast output file \"%s\"\n", ast_filename.c_str());
			exit(0);
		}
		ofs << "var ast = " << json_root << std::endl;
		ofs.close();
		exit(0);
	}
	return 0;
}
