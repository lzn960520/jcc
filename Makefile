.PHONY: all clean test vars deps

CPPFLAGS ?=
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
INCLUDES := jascal.tab.hpp
SOURCES := main.cpp CompileUnit.cpp Op2.cpp LiteralInt.cpp CmdLine.cpp \
	LiteralString.cpp Identifier.cpp Statements.cpp IfStatement.cpp \
	WhileStatement.cpp VariableDefination.cpp Type.cpp Function.cpp \
	ArgumentList.cpp Visibility.cpp Context.cpp FunctionCall.cpp \
	CallArgumentList.cpp lex.yy.cpp jascal.tab.cpp Exception.cpp \
	Block.cpp ASTNode.cpp Return.cpp
OBJS := $(patsubst %.cpp,objs/%.o,$(SOURCES))
DEPS := $(patsubst %.cpp,deps/%.d,$(SOURCES))

all: jcc

deps/%.d: %.cpp $(INCLUDES)
	$(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,objs/\1.o: $@ ,g' < $@.$$$$ > $@; \
	$(RM) $@.$$$$

objs/%.o:
	$(CXX) $(CPPFLAGS) -c -o $@ $*.cpp
	
include $(DEPS)

vars:
	@echo $(SOURCES)
	@echo $(DEPS)
	@echo $(OBJS)
	
deps: $(DEPS)

jcc: $(OBJS)
	g++ $(CPPFLAGS) -o $@ $(LDFLAGS) $^ $(LIBS)
	
jascal.tab.hpp: jascal.y
	bison -d jascal.y -v --report-file=bison-report.txt -o jascal.tab.cpp

jascal.tab.cpp: jascal.y
	bison -d jascal.y -v --report-file=bison-report.txt -o jascal.tab.cpp

lex.yy.cpp: jascal.l
	flex --outfile=lex.yy.cpp jascal.l

clean:
	rm -rf jascal.tab.cpp jascal.tab.hpp lex.yy.cpp jcc $(OBJS) $(DEPS) lex.txt ast.json bison-report.txt test.exe

test: jcc test.jas
	./jcc --dump-lex --dump-ast --llvm -o test.llvm test.jas
