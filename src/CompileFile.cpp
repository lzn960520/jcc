#include <unistd.h>

#include "CompileFile.h"
#include "Module.h"
#include "util.h"
#include "JsymFile.h"
#include "Context.h"
#include "Class.h"
#include "Symbol.h"
#include "Function.h"

Json::Value CompileFile::json() {
	Json::Value root;
	root["name"] = "file";
	root["usings"] = Json::Value(Json::arrayValue);
	int i = 0;
	for (std::list<std::string>::iterator it = usings.begin(); it != usings.end(); it++, i++)
		root["usings"][i] = *it;
	i = 0;
	root["modules"] = Json::Value(Json::arrayValue);
	for (std::list<Module*>::iterator it = modules.begin(); it != modules.end(); it++, i++)
		root["modules"][i] = (*it)->json();
	return root;
}

void CompileFile::gen(Context &context) {
	for (std::list<Module*>::iterator it = modules.begin(); it != modules.end(); it++)
		(*it)->gen(context);
	if (!entryPoint.empty()) {
		Class *mainClass = context.findClass(entryPoint);
		if (mainClass == NULL)
			throw SymbolNotFound("No such main class '" + entryPoint + "'");
		Symbol *mainFunc = mainClass->findSymbol("main");
		if (mainFunc == NULL)
			throw SymbolNotFound("Main class doesn't have main function");
		if (mainFunc->type != Symbol::STATIC_FUNCTION)
			throw InvalidType("'main' in main class isn't static function");
		std::vector<llvm::Type*> arg_type;
		arg_type.push_back(context.getBuilder().getInt32Ty());
		arg_type.push_back(context.getBuilder().getInt8PtrTy(0));
		llvm::Function *llvmFunc = context.createFunction(
				"main",
				llvm::FunctionType::get(
						context.getBuilder().getInt32Ty(),
						llvm::ArrayRef<llvm::Type*>(arg_type),
						false
				),
				NULL);
		context.getBuilder().CreateRet(context.getBuilder().CreateCall(mainFunc->data.static_function.function->getLLVMFunction(context)));
		context.endFunction();
	}
}

void CompileFile::genStruct(Context &context) {
	for (std::list<std::string>::iterator it = libs.begin(); it != libs.end(); it++) {
		std::string jsympath = resolveRelativePath(path, *it);
		if (access(jsympath.c_str(), R_OK))
			jsympath = "lib/" + *it;
		if (access(jsympath.c_str(), R_OK))
			throw CompileException("No such jsym file '" + *it + "'");
		JsymFile jsym(jsympath, true);
		jsym >> context;
	}
	for (std::list<std::string>::iterator it = usings.begin(); it != usings.end(); it++) {
		std::string &str = *it;
		if (str.rfind("::") == std::string::npos)
			continue;
		context.addAlias(str.substr(str.rfind("::") + 2), str);
	}
	for (std::list<Module*>::iterator it = modules.begin(); it != modules.end(); it++)
		(*it)->genStruct(context);
}
