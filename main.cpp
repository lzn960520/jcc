#include <unistd.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <errno.h>
#include "yyvaltypes.h"
#include <fstream>

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

static struct option long_opts[] = {
	{ "lex-only", no_argument, NULL, OPT_LEX_ONLY },
	{ "parse-only", no_argument, NULL, OPT_PARSE_ONLY },
	{ "dump-lex", optional_argument, NULL, OPT_DUMP_LEX},
	{ "dump-ast", optional_argument, NULL, OPT_DUMP_AST},
	{ "input", required_argument, NULL, OPT_INPUT_FILE }
};

int main(int argc, char * const argv[]) {
	// process cmdline
	const char *ast_output_filename = NULL;
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
		lex_only();
	if (opt_parse_only)
		yyparse();
	yyparse();

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
