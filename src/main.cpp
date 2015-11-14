#include <fstream>
#include <cerrno>
#include <cstdio>
#include <iostream>
#include "Context.h"
#include "exception.h"
#include "cmdline.h"
#include "Module.h"
#include "Output.h"
#include "JsymFile.h"
#include "util.h"
#include "compile.h"

// global
std::string html_filename;

// from flex
typedef void (*lex_output_func)(char *fmt, ...);
extern lex_output_func lex_output;
extern void lex_only(void *yyscanner);

// from bison
extern void yyparse();
extern std::list<Module*> modules;
extern std::list<std::string> usings;

// for cmdline process
#define OPT_LEX_ONLY 256
#define OPT_PARSE_ONLY 257
#define OPT_INPUT_FILE 258
#define OPT_DUMP_HTML 259
#define OPT_OUTPUT_FILE 260
#define OPT_COMPILE_ONLY 261
#define OPT_AS_ONLY 262
#define OPT_LLVM_ONLY 263

// for store lex txt
static std::string lex;

void _lex_output(char *fmt, ...) {
	static char buf[4096];
	va_list va;
	va_start(va, fmt);
	int i = vsnprintf(buf, sizeof(buf), fmt, va);
	if (i < 0 || i >= sizeof(buf))
		throw CompileException("token too long");
	va_end(va);
	lex += buf;
}

void _disable_lex_output(char *fmt, ...) {
}

int main(int argc, char * const argv[]) {
	// process cmdline
	CmdLine cmdline(argc, argv);
	cmdline.registerOpt(OPT_DUMP_HTML, "--dump-html", CmdLine::OPT_ARG);
	cmdline.registerOpt(OPT_LEX_ONLY, "--lex", CmdLine::NO_ARG);
	cmdline.registerOpt(OPT_PARSE_ONLY, "--parse", CmdLine::NO_ARG);
	cmdline.registerOpt(OPT_OUTPUT_FILE, "-o", CmdLine::REQUIRE_ARG);
	cmdline.registerOpt(OPT_COMPILE_ONLY, "-S", CmdLine::NO_ARG);
	cmdline.registerOpt(OPT_AS_ONLY, "-c", CmdLine::NO_ARG);
	cmdline.registerOpt(OPT_LLVM_ONLY, "--llvm", CmdLine::NO_ARG);

	bool opt_lex_only = false, opt_parse_only = false,
				opt_dump_html = false,
				opt_as_only = false, opt_compile_only = false, opt_llvm_only = false;
	std::string ast_filename, output_filename;
	std::list<std::string> input_files;
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
			input_files.push_back(opt.arg.c_str());
			break;
		case OPT_OUTPUT_FILE:
			if (!output_filename.empty()) {
				fprintf(stderr, "Can't specify two output files at the same time\n");
				exit(-1);
			}
			output_filename = opt.arg;
			break;
		case OPT_DUMP_HTML:
			if (opt_dump_html) {
				fprintf(stderr, "Can't specify two html output files at the same time\n");
				exit(-1);
			}
			opt_dump_html = true;
			html_filename = opt.arg;
			break;
		default:
			fprintf(stderr, "Unknown option %s\n", opt.opt.c_str());
			exit(-1);
			break;
		}
	}

	if (opt_lex_only + opt_parse_only + opt_llvm_only + opt_as_only + opt_compile_only > 1) {
		fprintf(stderr, "You can't specify two --xxx-only options at the same time\n");
		exit(-1);
	}

	bool success = true;
	do {
		try {
			for (std::list<std::string>::iterator it = input_files.begin(); it != input_files.end(); it++) {
				std::ifstream ifs(it->c_str());
				if (!ifs) {
					fprintf(stderr, "Can't open input file '%s'\n", it->c_str());
					continue;
				}
				std::list<Token*> tokens = tokenize(ifs);
				ifs.close();
				if (opt_lex_only)
					break;
				CompileFile *root = parse(tokens);
				if (opt_parse_only)
					break;
				{
					std::ofstream ofs(getTempName().c_str());
					genSym(root, ofs);
					ofs.close();
				}
				Context context(true);
				if (context.isDebug)
					context.initDWARF(*it);
				compile(root, context);
				{
					std::ofstream ofs(output_filename.c_str());
					outputLLVM(context.getModule(), ofs);
					ofs.close();
				}
			}
		} catch (CompileException &e) {
			fprintf(stderr, "Compile error: %s\n", e.message().c_str());
			success = false;
		}
	} while(0);

	return success ? 0 : -1;
}
