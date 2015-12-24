#include "LiteralString.h"
#include "exception.h"
#include "Context.h"
#include "Type.h"
#include "Class.h"

static inline int char2hex(const char a) {
	switch (a) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return a - '0';
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
		return a - 'a' + 10;
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
		return a - 'A' + 10;
	default:
		return -1;
	}
}
LiteralString::LiteralString(const char *ori_text) :
	text(strlen(ori_text), '\0'), ori_text(ori_text) {
	ori_text++;
	char *p;
	for (p = (char *) text.data(); *ori_text && (*ori_text) != '\"'; ori_text++, p++) {
		if (*ori_text != '\\')
			*p = *ori_text;
		else {
			switch (*(ori_text + 1)) {
			case 'a':
				*p = '\a';
				ori_text++;
				break;
			case 'b':
				*p = '\b';
				ori_text++;
				break;
			case 'f':
				*p = '\f';
				ori_text++;
				break;
			case 'n':
				*p = '\n';
				ori_text++;
				break;
			case 'r':
				*p = '\r';
				ori_text++;
				break;
			case 't':
				*p = '\t';
				ori_text++;
				break;
			case 'v':
				*p = '\v';
				ori_text++;
				break;
			case '\\':
				*p = '\\';
				ori_text++;
				break;
			case '\'':
				*p = '\'';
				ori_text++;
				break;
			case '\"':
				*p = '\"';
				ori_text++;
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
				ori_text++;
				*p = 0;
				while ((*ori_text <= '9') && (*ori_text >= '0'))
					*p = *p * 8 + *ori_text++ - '0';
				ori_text--;
				break;
			case 'x':
				if (char2hex(*(ori_text + 2)) == -1 || char2hex(*(ori_text + 3)) == -1)
					throw InvalidString("Invalid \\x escape");
				*p = (char2hex(*(ori_text + 2)) << 4) + char2hex(*(ori_text + 3));
				ori_text += 3;
				break;
			default:
				*p = *(++ori_text);
				break;
			}
		}
	}
	text = text.substr(0, p - text.data());
}

Json::Value LiteralString::json() {
	Json::Value root;
	root["name"] = "literal_string";
	root["text"] = Json::Value(ori_text);
	return root;
}

LiteralString::~LiteralString() {
}

llvm::GlobalVariable* Context::getGlobalString(const std::string &text) {
	if (!globalStrings.count(text)) {
		llvm::Value *str = getBuilder().CreateGlobalStringPtr(text);
		Class *strCls = findClass("string");
		std::vector<llvm::Constant*> tmp;
		tmp.push_back(strCls->getVtable(NULL));
		tmp.push_back((llvm::Constant *) getBuilder().CreatePtrToInt(str, getBuilder().getInt32Ty()));
		llvm::Constant *myStr = llvm::ConstantStruct::get((llvm::StructType *) strCls->getLLVMType(), llvm::ArrayRef<llvm::Constant*>(tmp));
		globalStrings[text] = new llvm::GlobalVariable(getModule(), strCls->getLLVMType(), false, llvm::GlobalVariable::PrivateLinkage, myStr);
	}
	return globalStrings[text];
}

llvm::Value* LiteralString::load(Context &context) {
	return context.getGlobalString(text);
}

llvm::Instruction* LiteralString::store(Context &context, llvm::Value *value) {
	throw NotAssignable("literal string");
}

Type* LiteralString::getType(Context &context) {
	return &Type::String;
}

Type* LiteralString::getTypeConstant() {
	return &Type::String;
}
