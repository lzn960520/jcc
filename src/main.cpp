#include <iostream>
#include <fstream>
#include <cerrno>
#include "Context.h"
#include "exception.h"
#include "cmdline.h"
#include "Module.h"
#include "Output.h"

// global
std::string input_filename;

// from flex
extern FILE *yyin;
extern FILE *lex_output;
extern void lex_only();

// from bison
extern void yyparse();
extern std::list<Module*> modules;

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
				fprintf(stderr, "Can't open input file \"%s\", errno %d\n", opt.arg.c_str(), errno);
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

			Context *context = new Context();
			if (input_filename.empty())
				context->initDWARF("stdin");
			else
				context->initDWARF(input_filename);

			// generate struct type
			for (std::list<Module*>::iterator it = modules.begin(); it != modules.end(); it++)
				(*it)->genStruct(*context);

			for (std::list<Module*>::iterator it = modules.begin(); it != modules.end(); it++)
				(*it)->gen(*context);

			context->DI->finalize();

			if (opt_llvm_only) {
				if (output_filename.empty() && input_filename.empty())
					outputLLVM(context->getModule(), std::cout);
				else if (output_filename.empty()) {
					std::string oname = input_filename.substr(0, input_filename.rfind('.')) + ".ll";
					std::ofstream sys_ofs(oname);
					outputLLVM(context->getModule(), sys_ofs);
					sys_ofs.close();
				} else {
					std::ofstream sys_ofs(output_filename);
					outputLLVM(context->getModule(), sys_ofs);
					sys_ofs.close();
				}
			}

			if (opt_compile_only) {
				if (output_filename.empty() && input_filename.empty())
					outputAssemble(context->getModule(), std::cout);
				else if (output_filename.empty()) {
					std::string oname = input_filename.substr(0, input_filename.rfind('.')) + ".s";
					std::ofstream sys_ofs(oname);
					outputAssemble(context->getModule(), sys_ofs);
					sys_ofs.close();
				} else {
					std::ofstream sys_ofs(output_filename);
					outputAssemble(context->getModule(), sys_ofs);
					sys_ofs.close();
				}
			}
		} catch (CompileException &e) {
			fprintf(stderr, "Compile error: %s\n", e.message().c_str());
		}
	} while(0);

	if (opt_dump_lex)
		fclose(lex_output);
	if (opt_dump_ast) {
		std::ofstream ofs(ast_filename);
		if (!ofs) {
			fprintf(stderr, "Can't open ast output file \"%s\"\n", ast_filename.c_str());
			exit(0);
		}
		Json::Value tmp;
		tmp["name"] = "file";
		tmp["modules"] = Json::Value(Json::arrayValue);
		int i = 0;
		for (std::list<Module*>::iterator it = modules.begin(); it != modules.end(); i++, it++)
			tmp["modules"][i] = (*it)->json();
		ofs << "var ast = " << tmp << std::endl;
		ofs.close();
		exit(0);
	}
	return 0;
}
