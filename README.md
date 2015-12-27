# JCC (Jascal Compiler)
---------------------------
## Compilation
* Requirements
    * Make
    * G++/Clang++
    * Bison
    * Flex
    * Python 2.7
    * libjsoncpp-dev
    * llvm-3.7-dev
    * pkg-config
* Tested on
    * Ubuntu Server 15.10/16.04 AMD64
    * Make 4.0-8.2
    * Clang++ 1:3.6-26ubuntu1
    * Bison 2:3.0.4.dfsg-1
    * Flex 2.5.39-8
    * Python 2.7.10-4ubuntu2
    * libjsoncpp-dev 0.10.5-1
    * llvm-3.7-dev 1:3.7-4ubuntu1
    * pkg-config 0.29-2ubuntu1
* How-to
    1. `make`

----------------------------
## Usage
* Command line parameters for jcc<br>
    `jcc [options] <input_file1> [input_file2] [input_file3] ...`<br>
    Options:
    * `--dump-html <html_filename>`<br>
        Output the token stream and AST to a HTML file to view
    * `--lex`<br>
        Do the tokenize(lexical analysis) stage only
    * `--parse`<br>
        Do the parse(syntax analysis) stage only
    * `--llvm`<br>
        Do the compile stage only. (Generate the llvm assembly file)
    * `-S`<br>
        Do the compile stage only. (Generate the target assembly file)
    * `-c`<br>
        Do the assemble stage only. (Generate the object file)
    * `-o <output_filename>`<br>
        Specifiy the output filename
	* `-fno-builtin`<br>
		Don't include the standard library automatically. Mainly used to compile standard library itself.
* Built-in tests<br>
    You can just run `make test`, it will compile the test cases in `tests` directory using option `jcc --dump-html tests/xxx.html --llvm -o tests/xxx.ll tests/xxx.jas` and then run `clang` to compile and link the `.ll` file against jascal libs' implementations (such as `lib/io.impl.ll`)<br>
    For eash test case, it will generate an `xxx` file which is the executable binary file of the source file, and an `xxx.html` file which could be opened in browser to see the token stream and the AST of the source file
	Additionally, if there is any test data for a test case (named `tests/xxx-testx.in` and `tests/xxx-testx.out`), the tester will automatically run the executable file and feed the `.in` file to it, then get the output and compare it with `.out` file. There will be a time limit which is set to 2 seconds. The comparison will ignore blank line. A `[TEST] xxx.in -> xxx ...pass` will be printed if the output match the `.out` file.
