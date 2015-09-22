.PHONY: all clean test

CCFLAGS ?= 
YYVAL_TYPES_H = yyvaltypes.h
YYVAL_TYPES_CPPS = CompileUnit.cpp Expression.cpp LiteralInt.cpp LiteralString.cpp Identifier.cpp Statements.cpp IfStatement.cpp

all: jcc

jcc: jascal.tab.c jascal.tab.h lex.yy.c main.cpp $(YYVAL_TYPES_H) $(YYVAL_TYPES_CPPS)
	g++ $(CCFLAGS) -o $@ -x c++ jascal.tab.c -x c++ lex.yy.c main.cpp $(YYVAL_TYPES_CPPS) -ljsoncpp

jascal.tab.h: jascal.y
	bison -d jascal.y -v --report-file=bison-report.txt

jascal.tab.c: jascal.y
	bison -d jascal.y -v --report-file=bison-report.txt

lex.yy.c: jascal.l
	flex jascal.l

clean:
	rm -rf jascal.tab.c jascal.tab.h jascal.tab.cc lex.yy.c jcc lex.txt ast.json bison-report.txt

test: jcc test.jas
	./jcc --dump-lex --dump-ast --input test.jas
