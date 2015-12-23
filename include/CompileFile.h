#ifndef _COMPILE_FILE_H_
#define _COMPILE_FILE_H_

#include <list>

#include "StructNode.h"
#include "exception.h"

class Module;
class CompileFile : public StructNode {
	std::list<std::string> usings;
	std::list<std::string> libs;
	std::list<Module*> modules;
	std::string path;
	std::string entryPoint;
public:
	CompileFile(const std::string &path) : path(path) {}
	inline void addUsing(const std::string &_using) {
		usings.push_back(_using);
	}
	inline void addLib(const std::string &lib) {
		libs.push_back(lib);
	}
	inline void addModule(Module *module) {
		modules.push_back(module);
	}
	inline void setEntry(const std::string &entryPoint) {
		if (entryPoint.empty())
			return;
		if (!this->entryPoint.empty())
			throw Redefination("Entry point");
		this->entryPoint = entryPoint;
	}
	Json::Value json() override;
	void gen(Context &context) override;
	void genStruct(Context &context) override;
	void writeJsymFile(std::ostream &os) override;
};

#endif
