.PHONY: all clean test vars deps flex bison update-html archive

CXX := clang++
CC := clang
LEX := flex
LLVM_COMPONENTS := all

CPPFLAGS ?=
CPPFLAGS += -Iinclude -frtti -g -DDEBUG
ifeq ($(DEBUG), 1)
CPPFLAGS += -g
endif
CPPFLAGS += -std=c++11
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
	Visibility.cpp Context.cpp FunctionCall.cpp Qualifier.cpp \
	CallArgumentList.cpp lex.yy.cc jascal.tab.cc Exception.cpp \
	Block.cpp ASTNode.cpp RepeatStatement.cpp Op1.cpp ArrayAccess.cpp \
	ArrayAccessor.cpp Symbol.cpp output.cpp New.cpp DebugInfo.cpp \
	ArrayDefinator.cpp Namespace.cpp Module.cpp Class.cpp MemberAccess.cpp \
	JsymFile.cpp MemberVariableDefination.cpp compile.cpp Tokenizer.cpp \
	Token.cpp CompileFile.cpp Parser.cpp
OBJS := $(patsubst %.cpp,objs/%.o,$(SOURCES))
OBJS := $(patsubst %.cc,objs/%.o,$(OBJS))
OBJS += objs/HtmlTemplate.o
DEPS := $(patsubst %.cpp,deps/%.d,$(SOURCES))
DEPS := $(patsubst %.cc,deps/%.d,$(DEPS))
PROG := jcc
TESTS := $(wildcard tests/*.jas)
TESTS_OUT := $(patsubst %.jas,%.ll,$(TESTS))

all: $(PROG)

deps/%.d: src/%.cpp | flex bison
	@echo "[DEP ] $< -> $@"
	@$(CXX) -MM $(CPPFLAGS) $< > $@.$$$$ || exit "$$?"; \
	sed 's,\($*\)\.o[ :]*,objs/\1.o $@: ,g' < $@.$$$$ > $@ || exit "$$?"; \
	$(RM) $@.$$$$ || exit "$$?"

deps/%.d: src/%.cc | flex bison
	@echo "[DEP ] $< -> $@"
	@$(CXX) -MM $(CPPFLAGS) $< > $@.$$$$ || exit "$$?"; \
	sed 's,\($*\)\.o[ :]*,objs/\1.o $@: ,g' < $@.$$$$ > $@ || exit "$$?"; \
	$(RM) $@.$$$$ || exit "$$?"

objs/%.o: 
	@echo "[CXX ] $< -> $@"
	@$(CXX) $(CPPFLAGS) -c -o $@ $<

include/jascal.tab.hpp src/jascal.tab.cc: src/jascal.yy
	@echo "[YACC] $<"
	@$(YACC) --defines=include/jascal.tab.hpp --report-file=bison-report.txt -o src/jascal.tab.cc $<

src/lex.yy.cc: src/jascal.l
	@echo "[LEX ] $< -> $@"
	@$(LEX) --outfile=$@ $<
	
include $(DEPS)

vars:
	@echo "$$(SOURCES)" $(SOURCES)
	@echo $(DEPS)
	@echo $(OBJS)
	
deps: $(DEPS)

$(PROG): $(OBJS)
	@echo "[LINK] $@"
	@$(CXX) -o $@ $(LDFLAGS) $^ $(LIBS)

bison: src/jascal.tab.cc include/jascal.tab.hpp

flex: src/lex.yy.cc

clean:
	@echo "CLEAN"
	@$(RM) -rf src/jascal.tab.cc include/jascal.tab.hpp src/lex.yy.cc \
		 $(PROG) $(OBJS) $(DEPS) bison-report.txt tests/*.txt tests/*.ll tests/*.json tests/*.o

test: $(TESTS_OUT)

tests/%.ll: tests/%.jas $(PROG)
	@echo "[JCC ] tests/$*.jas -> tests/$*.ll"
	@./$(PROG) --dump-html tests/$*.html --llvm -o $@ $< || rm -f tests/$*.json tests/$*.txt $@

objs/index-comb.html: codeview/index.html tools/HtmlCombiner/combiner.py
	@echo "[GEN ] $< -> $@"
	@./tools/HtmlCombiner/combiner.py $< $@
	@dd if=/dev/zero bs=1 count=1 >> $@ 2>/dev/null

objs/HtmlTemplate.o: objs/index-comb.html tools/bincc/bincc
	@echo "[GEN ] $< -> $@"
	@./tools/bincc/bincc $< $@ htmlTemplate

tools/bincc/bincc: tools/bincc/bincc.c
	@echo "[CC  ] $< -> $@"
	@$(CC) -o $@ $<

objs/output.o: src/output.cpp
	@echo "[G++ ] $< -> $@"
	@g++ $(CPPFLAGS) -c -o $@ $<

update-html:
	@rm -f objs/index-comb.html
	@make all

archive:
	@echo ARCHIVE
	@git archive --format tar.gz --prefix jcc/ HEAD -o jcc.tar.gz
