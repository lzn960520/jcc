#ifndef _JSYM_FILE_H_
#define _JSYM_FILE_H_

#include <string>
#include <cstdio>

class Context;
class Class;
class Module;
class JsymFile {
	std::string path;
	bool isR;
public:
	JsymFile(const std::string &path, bool read);
	void operator <<(Class *cls);
	void operator <<(Module *module);
	void operator >>(Context &context);
};

#endif
