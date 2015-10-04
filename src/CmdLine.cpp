#include <cstring>

#include "cmdline.h"
#include "exception.h"

CmdLine::CmdLine(int argc, char * const argv[]) :
		argc(argc), argv(argv), i(1) {}

void CmdLine::registerOpt(int id, const char *name, ArgumentType type) {
	if (id < 0)
		return;
	if (type == NEAR_ARG)
		near_arg_opts.push_back(std::make_pair(std::string(name), id));
	else
		opts[name] = std::make_pair(id, type);
}

int CmdLine::next(Option &opt) {
	if (i == argc)
		return -2;
	std::map<std::string, std::pair<int, ArgumentType> >::iterator it;
	if (argv[i][0] != '-') {
		opt.arg = argv[i];
		opt.opt = "";
		opt.id = -1;
		i++;
		return -1;
	}
	char *p = strchr(argv[i], '=');
	char *arg = p ? p + 1 : NULL;
	std::string optstr(argv[i], p ? p - argv[i] : strlen(argv[i]));
	it = opts.find(optstr);
	if (it == opts.end()) {
		std::string tmp(argv[i]);
		for (std::list<std::pair<std::string, int> >::iterator it = near_arg_opts.begin(); it != near_arg_opts.end(); it++)
			if (optstr.substr(0, it->first.length()) == it->first) {
				opt.opt = it->first;
				opt.arg = optstr.substr(it->first.length());
				opt.id = it->second;
				i++;
				return opt.id;
			}
		opt.arg = arg ? arg : "";
		opt.opt = argv[i];
		opt.id = -3;
		i++;
		return -3;
	} else {
		switch (it->second.second) {
		case NO_ARG:
			opt.opt = it->first;
			opt.arg = "";
			opt.id = it->second.first;
			i++;
			return opt.id;
		case OPT_ARG:
			opt.opt = it->first;
			opt.arg = arg ? arg : "";
			opt.id = it->second.first;
			i++;
			return opt.id;
		case REQUIRE_ARG:
			opt.opt = it->first;
			if (!arg && i == argc - 1)
				throw ArgumentException("No argument for option " + it->first);
			opt.arg = arg ? arg : argv[i + 1];
			opt.id = it->second.first;
			i += arg ? 1 : 2;
			return opt.id;
		}
	}
}
