.PHONY: all clean test vars deps flex bison

CXX := clang++
LEX := flex
LLVM_COMPONENTS := all

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
LIBS += -lLLVM-3.5
else
CPPFLAGS += `llvm-config-3.7 --cppflags`
LDFLAGS += `llvm-config-3.7 --ldflags`
#LIBS += `llvm-config-3.7 --libs $(LLVM_COMPONENTS)`
LIBS += -lLLVM-3.7
LIBS += `llvm-config-3.7 --system-libs $(LLVM_COMPONENTS)`
endif

CPPFLAGS += -frtti -fexceptions

# jascal
SOURCES := main.cpp Return.cpp Op2.cpp LiteralInt.cpp CmdLine.cpp \
	LiteralString.cpp Identifier.cpp Statements.cpp IfStatement.cpp \
	WhileStatement.cpp VariableDefination.cpp Type.cpp Function.cpp \
	Visibility.cpp Context.cpp FunctionCall.cpp \
	CallArgumentList.cpp lex.yy.cpp jascal.tab.cpp Exception.cpp \
	Block.cpp ASTNode.cpp RepeatStatement.cpp Op1.cpp ArrayAccess.cpp \
	ArrayAccessor.cpp Symbol.cpp Output.cpp New.cpp DebugInfo.cpp \
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
	@echo "CXX src/$*.cpp -> $@"
	@$(CXX) $(CPPFLAGS) -c -o $@ src/$*.cpp
	
include/jascal.tab.hpp: src/jascal.y
	$(YACC) -d src/jascal.y -v --report-file=bison-report.txt -o src/jascal.tab.cpp
	mv src/jascal.tab.hpp include/

src/jascal.tab.cpp: include/jascal.tab.hpp

include $(DEPS)

vars:
	@echo "$$(SOURCES)" $(SOURCES)
	@echo $(DEPS)
	@echo $(OBJS)
	
deps: $(DEPS)

$(PROG): $(OBJS)
	$(CXX) $(CPPFLAGS) -o $@ $(LDFLAGS) $^ $(LIBS)

bison: include/jascal.tab.hpp src/jascal.tab.cpp 

src/lex.yy.cpp: src/jascal.l
	$(LEX) --outfile=src/lex.yy.cpp src/jascal.l
	
flex: src/lex.yy.cpp

clean:
	$(RM) -rf src/jascal.tab.cpp include/jascal.tab.hpp src/lex.yy.cpp \
		 $(PROG) $(OBJS) $(DEPS) lex.txt bison-report.txt test.ll test.ll.s \
		 test.o test.exe test ast/ast.json

test: jcc test.jas
	./$(PROG) --llvm --dump-lex --dump-ast=ast/ast.json -o test.ll test.jas
