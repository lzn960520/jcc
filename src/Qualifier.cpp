#include "Qualifier.h"
#include "exception.h"

Qualifier::Qualifier() :
		_isConst(false), _isStatic(false), _isPublic(false), _isPrivate(false), _isProtected(false) {
}

Qualifier::~Qualifier() {
}

void Qualifier::setConst() {
	if (_isConst)
		throw CompileException("Two \"const\"");
	_isConst = true;
}

void Qualifier::setStatic() {
	if (_isStatic)
		throw CompileException("Two \"static\"");
	_isStatic = true;
}

void Qualifier::setPublic() {
	if (_isPublic || _isPrivate || _isProtected)
		throw CompileException("Two visibility specified");
	_isPublic = true;
}

void Qualifier::setPrivate() {
	if (_isPublic || _isPrivate || _isProtected)
		throw CompileException("Two visibility specified");
	_isPrivate = true;
}

void Qualifier::setProtected() {
	if (_isPublic || _isPrivate || _isProtected)
		throw CompileException("Two visibility specified");
	_isProtected = true;
}

Json::Value Qualifier::json() {
	Json::Value ans(Json::arrayValue);
	if (isConst())
		ans.append("const");
	if (isStatic())
		ans.append("static");
	if (isPublic())
		ans.append("public");
	if (isPrivate())
		ans.append("private");
	if (isProtected())
		ans.append("protected");
	return ans;
}
