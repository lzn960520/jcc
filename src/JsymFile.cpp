#include <fstream>

#include "JsymFile.h"
#include "Class.h"
#include "Function.h"
#include "MemberVariableDefination.h"
#include "exception.h"
#include "Type.h"
#include "VariableDefination.h"
#include "Identifier.h"
#include "Namespace.h"
#include "Module.h"
#include "Qualifier.h"
#include "CompileFile.h"
#include "Interface.h"

void Qualifier::writeJsymFile(std::ostream &os) {
	os.write(isStatic() ? "S" : "s", 1);
	os.write(isPublic() ? "U" : (isPrivate() ? "R" : "O"), 1);
	os.write(isConst() ? "C" : "c", 1);
}

void Type::writeJsymFile(std::ostream &os) {
	os << getMangleName();
}

void Function::writeJsymFile(std::ostream &os) {
	os.write("F", 1);
	qualifier->writeJsymFile(os);
	size_t len = getName().size();
	os.write((char *) &len, 4);
	os.write(getName().c_str(), len);
	if (return_type)
		return_type->writeJsymFile(os);
	else
		os.write("v", 1);
	for (arg_iterator it = arg_list.begin(); it != arg_list.end(); it++)
		it->first->writeJsymFile(os);
	os.write("EF", 2);
}

void Class::writeJsymFile(std::ostream &os) {
	assert(module == NULL);
	os.write(getMangleName().c_str(), 1);
	size_t len = getName().size();
	os.write((char *) &len, 4);
	os.write(getName().c_str(), len);
	if (extendsClass) {
		os.write("S", 1);
		len = extendsClass->getFullName().size();
		os.write((char *) &len, 4);
		os.write(extendsClass->getFullName().c_str(), len);
	}
	for (std::list<std::pair<Interface*, int> >::iterator it = implementsType.begin(); it != implementsType.end(); it++) {
		os.write("I", 1);
		len = it->first->getFullName().size();
		os.write((char *) &len, 4);
		os.write(it->first->getFullName().c_str(), len);
	}
	for (std::list<MemberNode*>::iterator it = list.begin(); it != list.end(); it++)
		(*it)->writeJsymFile(os);
	os.write("E", 1);
	os.write(getMangleName().c_str(), 1);
}

void Interface::writeJsymFile(std::ostream &os) {
	assert(module == NULL);
	os.write("I", 1);
	size_t len = getName().size();
	os.write((char *) &len, 4);
	os.write(getName().c_str(), len);
	for (std::list<MemberNode*>::iterator it = list.begin(); it != list.end(); it++)
		(*it)->writeJsymFile(os);
	os.write("EI", 2);
}

void MemberVariableDefination::writeJsymFile(std::ostream &os) {
	os.write("V", 1);
	qualifier->writeJsymFile(os);
	vars->type->writeJsymFile(os);
	size_t len = vars->list.size();
	os.write((char *) &len, 4);
	for (std::list<std::pair<Identifier*, Expression*> >::iterator it = vars->list.begin(); it != vars->list.end(); it++) {
		size_t len = it->first->getName().size();
		os.write((char *) &len, 4);
		os.write(it->first->getName().c_str(), len);
	}
	os.write("EV", 2);
}

void Module::writeJsymFile(std::ostream &os) {
	os.write("M", 1);
	std::string tmp = getFullName();
	while (!tmp.empty()) {
		size_t i = tmp.find(':');
		if (i == std::string::npos)
			i = tmp.size();
		os.write("N", 1);
		os.write((char *) &i, 4);
		os.write(tmp.c_str(), i);
		if (i == tmp.size())
			tmp = "";
		else
			tmp = tmp.substr(i + 2);
	}
	for (std::list<StructNode*>::iterator it = list.begin(); it != list.end(); it++)
		(*it)->writeJsymFile(os);
	os.write("EM", 2);
}

void CompileFile::writeJsymFile(std::ostream &os) {
	for (std::list<Module*>::iterator it = modules.begin(); it != modules.end(); it++)
		(*it)->writeJsymFile(os);
}

static Qualifier* readQualifier(char *&p, char *end) {
	char * const save_p = p;
	Qualifier *qul = new Qualifier();
	try {
		if (p >= end || (*p != 'S' && *p != 's'))
			throw CompileException("Error reading Jsym file");
		if (*p == 'S')
			qul->setStatic();
		p++;
		if (p >= end || (*p != 'U' && *p != 'R' && *p != 'O'))
			throw CompileException("Error reading Jsym file");
		if (*p == 'U')
			qul->setPublic();
		else if (*p == 'R')
			qul->setPrivate();
		else
			qul->setProtected();
		p++;
		if (p >= end || (*p != 'C' && *p != 'c'))
			throw CompileException("Error reading Jsym file");
		if (*p == 'C')
			qul->setConst();
		p++;
		return qul;
	} catch(...) {
		delete qul;
		p = save_p;
		throw;
	}
}

static unsigned int readUint(char *&p, char *end) {
	char * const save_p = p;
	unsigned int i = 0;
	if (p >= end || *p < '0' || *p > '9')
		throw CompileException("Error reading Jsym file");
	while (p < end && *p >= '0' && *p <= '9')
		i = i * 10 + *p++ - '0';
	return i;
}

static Identifier* readIdentifier(char *&p, char *end) {
	char *save_p = p;
	if (p >= end)
		throw CompileException("Error reading Jsym file");
	size_t len = *(unsigned int *) p;
	p += 4;
	if (p + len > end) {
		p = save_p;
		throw CompileException("Error reading Jsym file");
	}
	Identifier *ans = new Identifier(std::string(p, len).c_str());
	p += len;
	return ans;
}

static const std::string readMangleName(char *&p, char *end) {
	char * const save_p = p;
	std::string ans;
	try {
		if (p >= end)
			throw CompileException("Error reading Jsym file");
		if (p < end + 1 && p[0] == '_' && p[1] == 'F') {
			// Function
			return "";
		} else {
			// Class or Interface
			char type = *p++;
			while (p < end && *p != type) {
				if (*p != 'N')
					throw CompileException("Error reading Jsym file");
				p++;
				size_t len = 0;
				while (p < end && isdigit(*p))
					len = len * 10 + (*p++ - '0');
				if (len == 0 || p + len >= end)
					throw CompileException("Error reading Jsym file");
				ans += std::string(p, len) + "::";
				p += len;
			}
			if (p >= end || *p != type)
				throw CompileException("Error reading Jsym file");
			p++;
			size_t len = 0;
			while (p < end && isdigit(*p))
				len = len * 10 + (*p++ - '0');
			if (len == 0 || p + len >= end)
				throw CompileException("Error reading Jsym file");
			ans += std::string(p, len);
			p += len;
			return ans;
		}
	} catch (...) {
		p = save_p;
		throw;
	}
}

static Type* readType(char *&p, char *end) {
	char * const save_p = p;
	try {
		if (p >= end)
			throw CompileException("Error reading Jsym file");
		switch (*p) {
		case 'b':
			p++;
			return new Type(Type::BYTE, false);
		case 'B':
			p++;
			return new Type(Type::BYTE, true);
		case 's':
			p++;
			return new Type(Type::SHORT, false);
		case 'S':
			p++;
			return new Type(Type::SHORT, true);
		case 'i':
			p++;
			return new Type(Type::INT, false);
		case 'I':
			p++;
			return new Type(Type::INT, true);
		case 'c':
			p++;
			return new Type(Type::CHAR);
		case 'f':
			p++;
			return new Type(Type::FLOAT);
		case 'd':
			p++;
			return new Type(Type::DOUBLE);
		case 'a':
			p++;
			return new Type(Type::STRING);
		case 't':
			p++;
			return new Type(Type::BOOL);
		case 'v':
			p++;
			return NULL;
		case 'A': {
			p++;
			size_t n_dim = readUint(p, end);
			std::vector<std::pair<int, int> > dims(n_dim);
			for (size_t i = 0; i < n_dim; i++) {
				if (p >= end)
					throw CompileException("Error reading Jsym file");
				if (*p == 'D') {
					p++;
					dims[i].first = 0;
					dims[i].second = 0;
				} else if (*p == 'S') {
					p++;
					size_t dim = readUint(p, end);
					dims[i].first = 0;
					dims[i].second = dim - 1;
				} else
					throw CompileException("Error reading Jsym file");
			}
			return new Type(readType(p, end), dims);
		}
		case 'N':
		case 'J':
		case 'C': {
			return new Type(new Identifier(readMangleName(p, end).c_str()));
		}
		default:
			throw CompileException("Error reading Jsym file");
		}
	} catch (...) {
		p = save_p;
		throw;
	}
}

static Namespace* readNamespace(char *&p, char *end) {
	char * const save_p = p;
	Namespace *ans = NULL;
	try {
		if (p >= end || *p != 'N')
			throw CompileException("Error reading Jsym file");
		p++;
		Namespace *ans = new Namespace(readIdentifier(p, end));
		while (p < end && *p == 'N') {
			p++;
			ans->push_back(readIdentifier(p, end));
		}
		return ans;
	} catch(...) {
		if (ans)
			delete ans;
		p = save_p;
		throw;
	}
}

static Function* readFunction(char *&p, char *end) {
	char * const save_p = p;
	Qualifier *qul = NULL;
	Identifier *id = NULL;
	Type *return_type = NULL;
	std::list<std::pair<Type*, Identifier*> > *arg_list = new std::list<std::pair<Type*, Identifier*> >();
	try {
		if (p >= end || *p != 'F')
			throw CompileException("Error reading Jsym file");
		p++;
		qul = readQualifier(p, end);
		id = readIdentifier(p, end);
		return_type = readType(p, end);
		while (p < end && *p != 'E')
			arg_list->push_back(std::pair<Type*, Identifier*>(readType(p, end), NULL));
		if (p + 1 >= end || *p != 'E' || *(p + 1) != 'F')
			throw CompileException("Error reading Jsym file");
		p += 2;
		return new Function(qul, return_type, id, arg_list, (llvm::Function*) NULL);
	} catch (...) {
		delete qul;
		if (id)
			delete id;
		for (std::list<std::pair<Type*, Identifier*> >::iterator it = arg_list->begin(); it != arg_list->end(); it++)
			delete it->first;
		delete arg_list;
		p = save_p;
		throw;
	}
}

static MemberVariableDefination* readMemberVariableDefination(char *&p, char *end) {
	char * const save_p = p;
	Qualifier *qul = NULL;
	Type *type = NULL;
	std::list<std::pair<Identifier*, Expression*> > *var_list = new std::list<std::pair<Identifier*, Expression*> >();
	try {
		if (p >= end || *p != 'V')
			throw CompileException("Error reading Jsym file");
		p++;
		qul = readQualifier(p, end);
		type = readType(p, end);
		if (p + 4 >= end)
			throw CompileException("Error reading Jsym file");
		size_t n_var = *(unsigned int *) p;
		p += 4;
		for (size_t i = 0; i < n_var; i++)
			var_list->push_back(std::pair<Identifier*, Expression*>(readIdentifier(p, end), NULL));
		if (p + 1 >= end || *p != 'E' || *(p + 1) != 'V')
			throw CompileException("Error reading Jsym file");
		p += 2;
		return new MemberVariableDefination(qul, new VariableDefination(type, var_list));
	} catch(...) {
		if (qul)
			delete qul;
		for (std::list<std::pair<Identifier*, Expression*> >::iterator it = var_list->begin(); it != var_list->end(); it++)
			delete it->first;
		delete var_list;
		p = save_p;
		throw;
	}
}

static Class* readClass(char *&p, char *end) {
	char * const save_p = p;
	Identifier *id = NULL;
	Identifier *extends = NULL;
	std::list<MemberNode*> *defs = new std::list<MemberNode*>();
	std::list<Identifier*> *implements = new std::list<Identifier*>();
	try {
		if (p >= end || *p != 'C')
			throw CompileException("Error reading Jsym file");
		p++;
		id = readIdentifier(p, end);
		if (*p == 'S') {
			p++;
			extends = readIdentifier(p, end);
		}
		while (p < end && *p == 'I') {
			p++;
			implements->push_back(readIdentifier(p, end));
		}
		if (p >= end)
			throw CompileException("Error reading Jsym file");
		while (p < end && *p != 'E') {
			switch (*p) {
			case 'F':
				defs->push_back(readFunction(p, end));
				break;
			case 'V':
				defs->push_back(readMemberVariableDefination(p, end));
				break;
			default:
				throw CompileException("Error reading Jsym file");
			}
		}
		if (p + 1 >= end || *p != 'E' || *(p + 1) != 'C')
			throw CompileException("Error reading Jsym file");
		p += 2;
		return new Class(id, extends, implements, defs);
	} catch(...) {
		if (id)
			delete id;
		if (extends)
			delete extends;
		for (std::list<Identifier*>::iterator it = implements->begin(); it != implements->end(); it++)
			delete *it;
		delete implements;
		for (std::list<MemberNode*>::iterator it = defs->begin(); it != defs->end(); it++)
			delete *it;
		delete defs;
		p = save_p;
		throw;
	}
}

Module *readModule(char *&p, char *end) {
	char * const save_p = p;
	Namespace *ns = NULL;
	std::list<StructNode*> *list = new std::list<StructNode*>();
	try {
		if (p >= end || *p != 'M')
			throw CompileException("Error reading Jsym file");
		p++;
		ns = readNamespace(p, end);
		while (p < end && *p != 'E') {
			switch (*p) {
			case 'C':
				list->push_back(readClass(p, end));
				break;
			default:
				throw CompileException("Error reading Jsym file");
			}
		}
		if (p + 1 >= end || *p != 'E' || *(p + 1) != 'M')
			throw CompileException("Error reading Jsym file");
		p += 2;
		return new Module(ns, list);
	} catch(...) {
		for (std::list<StructNode*>::iterator it = list->begin(); it != list->end(); it++)
			delete *it;
		delete list;
		p = save_p;
		throw;
	}
}

JsymFile::JsymFile(const std::string &name, bool read) :
		path(name), isR(read) {
}

void JsymFile::operator<<(Class *cls) {
	if (path.empty() || isR)
		return;
	std::ofstream ofs(path.c_str(), std::ofstream::binary | std::ofstream::app);
	cls->writeJsymFile(ofs);
	ofs.close();
}

void JsymFile::operator<<(Module *module) {
	if (path.empty() || isR)
		return;
	std::ofstream ofs(path.c_str(), std::ofstream::binary | std::ofstream::app);
	module->writeJsymFile(ofs);
	ofs.close();
}

void JsymFile::operator>>(Context &context) {
	if (path.empty() || !isR)
		return;
	FILE *f = fopen(path.c_str(), "rb");
	if (f == NULL)
		throw CompileException("Can't open jsym file '" + path + "'");
	std::fseek(f, 0, SEEK_END);
	size_t len = std::ftell(f);
	std::fseek(f, 0, SEEK_SET);
	char *buf = (char *) std::malloc(len), *p = buf;
	std::fread(buf, len, 1, f);
	std::fclose(f);
	std::list<Module*> tmp;
	try {
		while (p < buf + len)
			tmp.push_back(readModule(p, buf + len));
		for (std::list<Module*>::iterator it = tmp.begin(); it != tmp.end(); it++)
			context.addModule(*it);
		std::free(buf);
	} catch(...) {
		std::free(buf);
		for (std::list<Module*>::iterator it = tmp.begin(); it != tmp.end(); it++)
			delete *it;
		throw;
	}
}
