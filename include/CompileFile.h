#ifndef _COMPILE_FILE_H_
#define _COMPILE_FILE_H_

#include <list>

#include "StructNode.h"

class Module;
class CompileFile : public StructNode {
	std::list<std::string> usings;
	std::list<Module*> modules;
	std::string path;
public:
	CompileFile(const std::string &path) : path(path) {}
	inline void addUsing(const std::string &_using) {
		usings.push_back(_using);
	}
	inline void addModule(Module *module) {
		modules.push_back(module);
	}
	Json::Value json() override;
	void gen(Context &context) override;
	void genStruct(Context &context) override;
	void writeJsymFile(std::ostream &os) override;
};

#endif
