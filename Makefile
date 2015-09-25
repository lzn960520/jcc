.PHONY: all clean test

OS_NAME = $(shell uname -o | tr '[A-Z]' '[a-z]')

# libjsoncpp
ifeq ($(OS_NAME), cygwin)
CCFLAGS += 
LIBS += -ljsoncpp
else
CCFLAGS += `pkg-config --cflags jsoncpp`
LIBS += `pkg-config --libs jsoncpp`
endif

# llvm
ifeq ($(OS_NAME), cygwin)
CCFLAGS +=
LIBS += -lLLVM-3.5
else
CCFLAGS += -I/usr/include/llvm-3.6 -I/usr/include/llvm-c-3.6
LIBS += -L/usr/lib/llvm-3.6/lib -lLLVM-3.6
endif

# jascal
YYVAL_TYPES_H = yyvaltypes.h
YYVAL_TYPES_CPPS = CompileUnit.cpp Expression.cpp LiteralInt.cpp \
	LiteralString.cpp Identifier.cpp Statements.cpp IfStatement.cpp \
	WhileStatement.cpp VariableDefination.cpp Type.cpp Function.cpp \
	ArgumentList.cpp Visibility.cpp Context.cpp FunctionCall.cpp \
	CallArgumentList.cpp

all: jcc

jcc: jascal.tab.c jascal.tab.h lex.yy.c main.cpp exception.h $(YYVAL_TYPES_H) $(YYVAL_TYPES_CPPS)
	g++ $(CCFLAGS) -std=gnu++11 -o $@ -x c++ jascal.tab.c -x c++ lex.yy.c main.cpp $(YYVAL_TYPES_CPPS) $(LIBS)

jascal.tab.h: jascal.y
	bison -d jascal.y -v --report-file=bison-report.txt

jascal.tab.c: jascal.y
	bison -d jascal.y -v --report-file=bison-report.txt

lex.yy.c: jascal.l
	flex jascal.l

clean:
	rm -rf jascal.tab.c jascal.tab.h jascal.tab.cc lex.yy.c jcc lex.txt ast.json bison-report.txt test.exe

test: jcc test.jas
	./jcc --dump-lex --dump-ast --input test.jas --output test
