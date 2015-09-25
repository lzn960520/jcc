#include <unistd.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <errno.h>
#include "yyvaltypes.h"
#include <fstream>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/PassManager.h>
#include <llvm/IR/IRPrintingPasses.h>
#include "context.h"
#include "exception.h"
#include <llvm/MC/MCContext.h>
#include <llvm/MC/MCAsmBackend.h>
#include <llvm/MC/MCAssembler.h>
#include <llvm/MC/MCAsmInfoELF.h>
#include <llvm/MC/MCRegisterInfo.h>
#include <llvm/MC/MCObjectFileInfo.h>
#include <llvm/Target/TargetMachine.h>
#include <wait.h>

// global
const char *input_filename = NULL;

// from flex
extern FILE *yyin;
extern FILE *lex_output;
extern void lex_only();

// from bison
extern void yyparse();
extern ASTNode *root;

// for cmdline process
#define OPT_LEX_ONLY 1
#define OPT_PARSE_ONLY 2
#define OPT_INPUT_FILE 3
#define OPT_DUMP_LEX 4
#define DEFAULT_LEX_OUTPUT "lex.txt"
#define OPT_DUMP_AST 5
#define DEFAULT_AST_OUTPUT "ast.json"
#define OPT_OUTPUT_FILE 6

static struct option long_opts[] = {
	{ "lex-only", no_argument, NULL, OPT_LEX_ONLY },
	{ "parse-only", no_argument, NULL, OPT_PARSE_ONLY },
	{ "dump-lex", optional_argument, NULL, OPT_DUMP_LEX},
	{ "dump-ast", optional_argument, NULL, OPT_DUMP_AST},
	{ "input", required_argument, NULL, OPT_INPUT_FILE },
	{ "output", required_argument, NULL, OPT_OUTPUT_FILE }
};

int main(int argc, char * const argv[]) {
	// process cmdline
	const char *ast_output_filename = NULL;
	const char *output_filename = NULL;
	bool opt_lex_only = false, opt_parse_only = false, opt_dump_lex = false, opt_dump_ast = false;
	{
		int opt;
		while ((opt = getopt_long(argc, argv, "", long_opts, NULL)) != -1) {
			switch (opt) {
			case OPT_LEX_ONLY:
				opt_lex_only = true;
				break;
			case OPT_PARSE_ONLY:
				opt_parse_only = true;
				break;
			case OPT_INPUT_FILE:
				if (yyin != NULL) {
					fprintf(stderr, "Can't specify two input files at the same time\n");
					exit(0);
				}
				yyin = fopen(optarg, "r");
				if (yyin == NULL) {
					fprintf(stderr, "Can't open input file \"%s\", errno %d\n", optarg, errno);
					exit(0);
				}
				input_filename = optarg;
				break;
			case OPT_OUTPUT_FILE:
				if (output_filename != NULL) {
					fprintf(stderr, "Can't specify two output files at the same time\n");
					exit(0);
				}
				output_filename = optarg;
				break;
			case OPT_DUMP_LEX:
				opt_dump_lex = true;
				if (lex_output != NULL) {
					fprintf(stderr, "Can't specify two lex output file\n");
					exit(0);
				}
				if (optarg == NULL)
					lex_output = fopen(DEFAULT_LEX_OUTPUT, "w");
				else
					lex_output = fopen(optarg, "w");
				if (lex_output == NULL) {
					fprintf(stderr, "Can't open lex output file \"%s\"\n", optarg == NULL ? DEFAULT_LEX_OUTPUT : optarg);
					exit(0);
				}
				break;
			case OPT_DUMP_AST:
				opt_dump_ast = true;
				if (ast_output_filename != NULL) {
					fprintf(stderr, "Can't specify two ast output file\n");
					exit(0);
				}
				ast_output_filename = optarg == NULL ? DEFAULT_AST_OUTPUT : optarg;
				break;
			default:
				fprintf(stderr, "Unknown option %s\n", argv[optind - 1]);
				exit(0);
				break;
			}
		}
	}

	if (opt_lex_only + opt_parse_only > 1) {
		fprintf(stderr, "You can't specify two --xxx-only options at the same time\n");
		exit(0);
	}

	if (opt_lex_only)
		try {
			lex_only(); // lex only
		} catch (CompileException &e) {
			fprintf(stderr, "Compile error: %s\n", e.message().c_str());
		}
	else if (opt_parse_only)
		try {
			yyparse(); // parse only
		} catch (CompileException &e) {
			root = NULL;
			fprintf(stderr, "Compile error: %s\n", e.message().c_str());
		}
	else {
		try {
			// normal compile
			yyparse();
		} catch (CompileException &e) {
			root = NULL;
			fprintf(stderr, "Compile error: %s\n", e.message().c_str());
		}

		try {
			if (root) {
				// call llvm to generate code
				Context *context = new Context();
				root->gen(*context);
				context->getBuilder().CreateRetVoid();

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
					execlp("llc", "llc", "-filetype=obj", "-o", "tmp.o", "-", NULL);
				}
				close(pipe_me_llvm_as[0]);
				close(pipe_llvm_as_llc[0]);
				close(pipe_llvm_as_llc[1]);
				{
					llvm::PassManager passManager;
					llvm::raw_fd_ostream ofs(pipe_me_llvm_as[1], false);
					passManager.add(llvm::createPrintModulePass(ofs));
					passManager.run(context->getModule());
					ofs.flush();
					ofs.close();
				}

				// wait for llc terminate
				waitpid(pid, NULL, 0);

				// exec gcc to build executable
				std::string cmdline = "gcc";
				if (output_filename) {
					cmdline += " -o ";
					cmdline += output_filename;
				}
				cmdline += " tmp.o";
				system(cmdline.c_str());

				unlink("tmp.o");
			}
		} catch (CompileException &e) {
			fprintf(stderr, "Compile error: %s\n", e.message().c_str());
		}
	}

	if (opt_dump_lex)
		fclose(lex_output);
	if (opt_dump_ast && root) {
		Json::Value json_root = root->json();
		std::ofstream ofs(ast_output_filename);
		if (!ofs) {
			fprintf(stderr, "Can't open ast output file \"%s\"\n", ast_output_filename);
			exit(0);
		}
		ofs << json_root << std::endl;
		ofs.close();
		exit(0);
	}
	return 0;
}
