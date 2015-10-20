#ifndef _CMDLINE_H_
#define _CMDLINE_H_

#include <map>
#include <string>
#include <list>

class CmdLine {
	const char * const *argv;
	const int argc;
	int i;
public:
	enum ArgumentType {
		NO_ARG,
		OPT_ARG,
		REQUIRE_ARG,
		NEAR_ARG
	};
private:
	std::list<std::pair<std::string, int> > near_arg_opts;
	std::map<std::string, std::pair<int, ArgumentType> > opts;
public:
	struct Option {
		std::string opt, arg;
		int id;
	};
	CmdLine(int argc, char * const argv[]);
	void registerOpt(int id, const char *name, ArgumentType type);
	int next(Option &opt);
};
#endif
