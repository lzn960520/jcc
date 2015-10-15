.PHONY: all clean test vars deps flex bison

LEX := flex

CPPFLAGS ?=
CPPFLAGS += -Iinclude -frtti
ifeq ($(DEBUG), 1)
CPPFLAGS += -g
endif
CPPFLAGS += -std=gnu++11
OS_NAME = $(shell uname -o | tr '[A-Z]' '[a-z]')

# libjsoncpp
ifeq ($(OS_NAME), cygwin)
CPPFLAGS += 
LIBS += -ljsoncpp
else
CPPFLAGS += `pkg-config --cflags jsoncpp`
LIBS += `pkg-config --libs jsoncpp`
endif

# llvm
ifeq ($(OS_NAME), cygwin)
CPPFLAGS +=
LIBS += -lLLVM-3.5
else
CPPFLAGS += -I/usr/include/llvm-3.6 -I/usr/include/llvm-c-3.6
LDFLAGS += -L/usr/lib/llvm-3.6/lib
LIBS += -lLLVM-3.6
endif

# jascal
SOURCES := main.cpp Return.cpp Op2.cpp LiteralInt.cpp CmdLine.cpp \
	LiteralString.cpp Identifier.cpp Statements.cpp IfStatement.cpp \
	WhileStatement.cpp VariableDefination.cpp Type.cpp Function.cpp \
	ArgumentList.cpp Visibility.cpp Context.cpp FunctionCall.cpp \
	CallArgumentList.cpp lex.yy.cpp jascal.tab.cpp Exception.cpp \
	Block.cpp ASTNode.cpp RepeatStatement.cpp Op1.cpp ArrayAccess.cpp \
	ArrayAccessor.cpp Symbol.cpp \
	ArrayDefinator.cpp Namespace.cpp Module.cpp Class.cpp
OBJS := $(patsubst %.cpp,objs/%.o,$(SOURCES))
DEPS := $(patsubst %.cpp,deps/%.d,$(SOURCES))
PROG := jcc

all: $(PROG)

deps/%.d: src/%.cpp | flex bison
	$(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,objs/\1.o: $@ ,g' < $@.$$$$ > $@; \
	$(RM) $@.$$$$

objs/%.o:
	$(CXX) $(CPPFLAGS) -c -o $@ src/$*.cpp
	
include $(DEPS)

vars:
	@echo $(SOURCES)
	@echo $(DEPS)
	@echo $(OBJS)
	
deps: $(DEPS)

$(PROG): $(OBJS)
	g++ $(CPPFLAGS) -o $@ $(LDFLAGS) $^ $(LIBS)
	
include/jascal.tab.hpp: src/jascal.y
	$(YACC) -d src/jascal.y -v --report-file=bison-report.txt -o src/jascal.tab.cpp
	mv src/jascal.tab.hpp include/

src/jascal.tab.cpp: include/jascal.tab.hpp

bison: include/jascal.tab.hpp src/jascal.tab.cpp 

src/lex.yy.cpp: src/jascal.l
	$(LEX) --outfile=src/lex.yy.cpp src/jascal.l
	
flex: src/lex.yy.cpp

clean:
	$(RM) -rf src/jascal.tab.cpp include/jascal.tab.hpp src/lex.yy.cpp \
		 $(PROG) $(OBJS) $(DEPS) lex.txt bison-report.txt test.llvm test.llvm.s \
		 test.o test.exe test ast/ast.json

test: jcc test.jas
	./$(PROG) --llvm --dump-lex --dump-ast=ast/ast.json -o test.llvm test.jas
