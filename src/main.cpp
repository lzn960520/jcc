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

// global
std::string input_filename;
std::string html_filename;

// from flex
extern FILE *yyin;
typedef void (*lex_output_func)(char *fmt, ...);
extern lex_output_func lex_output;
extern void lex_only();

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
				exit(-1);
			}
			yyin = fopen(opt.arg.c_str(), "r");
			if (yyin == NULL) {
				fprintf(stderr, "Can't open input file \"%s\", errno %d\n", opt.arg.c_str(), errno);
				exit(-1);
			}
			input_filename = opt.arg.c_str();
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

	// normalize filename
	if (output_filename.empty() && input_filename.empty())
		output_filename = "stdout";
	else if (output_filename.empty()) {
		if (opt_llvm_only)
			output_filename = changeExtName(input_filename, "ll");
		else if (opt_as_only)
			output_filename = changeExtName(input_filename, "o");
		else if (opt_compile_only)
			output_filename = changeExtName(input_filename, "S");
	}
	if (input_filename.empty())
		input_filename = "stdin";

	bool success = true;
	do {
		try {
			// prepare lex_output
			if (opt_dump_html)
				lex_output = _lex_output;
			else
				lex_output = _disable_lex_output;
			if (opt_lex_only) {
				lex_only();
				break;
			}
			yyparse();
			if (opt_parse_only)
				break;

			Context *context = new Context(input_filename, true);

			// import jsym
			for (std::list<std::string>::iterator it = usings.begin(); it != usings.end(); it++) {
				if (input_filename.empty()) {
					JsymFile jsymFile(*it, true);
					jsymFile >> *context;
				} else if (input_filename.rfind('/') == std::string::npos) {
					JsymFile jsymFile(*it, true);
					jsymFile >> *context;
				} else {
					JsymFile jsymFile(input_filename.substr(0, input_filename.rfind('/') + 1) + *it, true);
					jsymFile >> *context;
				}
			}

			// generate struct type
			for (std::list<Module*>::iterator it = modules.begin(); it != modules.end(); it++) {
				context->addModule(*it);
				context->getJsymFile() << (*it);
			}

			for (std::list<Module*>::iterator it = modules.begin(); it != modules.end(); it++)
				(*it)->gen(*context);

			context->DI->finalize();

			if (opt_llvm_only) {
				if (output_filename == "stdout")
					outputLLVM(context->getModule(), std::cout);
				else {
					std::ofstream sys_ofs(output_filename);
					outputLLVM(context->getModule(), sys_ofs);
					sys_ofs.close();
				}
			}

			if (opt_compile_only) {
				if (output_filename == "stdout")
					outputAssemble(context->getModule(), std::cout);
				else {
					std::ofstream sys_ofs(output_filename);
					outputAssemble(context->getModule(), sys_ofs);
					sys_ofs.close();
				}
			}
		} catch (CompileException &e) {
			fprintf(stderr, "Compile error: %s\n", e.message().c_str());
			success = false;
		}
	} while(0);

	if (opt_dump_html) {
		if (html_filename.empty())
			html_filename = changeExtName(output_filename, "html");
		std::ofstream ofs(html_filename.c_str());
		if (!ofs) {
			fprintf(stderr, "Can't open html output file %s\n", html_filename.c_str());
			exit(-1);
		}
		Json::Value tmp;
		tmp["name"] = "file";
		tmp["modules"] = Json::Value(Json::arrayValue);
		int i = 0;
		for (std::list<Module*>::iterator it = modules.begin(); it != modules.end(); i++, it++)
			tmp["modules"][i] = (*it)->json();
		outputHtml(lex, tmp, ofs);
		ofs.close();
	}
	return success ? 0 : -1;
}
