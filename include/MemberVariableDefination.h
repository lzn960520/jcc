#ifndef _MEMBER_VARIABLE_DEFINATION_H_
#define _MEMBER_VARIABLE_DEFINATION_H_

#include "MemberNode.h"

class Qualifier;
class VariableDefination;
class MemberVariableDefination : public MemberNode {
	Qualifier *qualifier;
	VariableDefination *vars;
public:
	MemberVariableDefination(Qualifier *qualifier, VariableDefination *vars);
	~MemberVariableDefination();
	Json::Value json() override;
	void gen(Context &context) override;
	void genStruct(Context &context) override;
	void writeJsymFile(FILE *f) override;
};

#endif
