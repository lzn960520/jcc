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
    jcc [options] &lt;input_file1&gt; [input_file2] [input_file3] ...<br>
    Options:
    * `--dump-html <html_filename>`<br>
        Output the token stream and AST to a HTML file to view
    * `--lex`<br>
        Do the tokenize(lexical analysis) stage only
    * `--parse`<br>
        Do the parse(syntax analysis) stage only
    * `--llvm`<br>
        Do the compile stage only. (Get the llvm assembly file)
    * `-S`<br>
        Do the compile stage only. (Get the target assembly file)
    * `-c`<br>
        Do the assemble stage only. (Get the object file)
    * `-o <output_filename>`<br>
        Specifiy the output filename
* Build-in tests<br>
    You can just run `make test`, it will compile the test cases in `tests` directory using option `jcc --dump-html tests/xxx.html --llvm -o tests/xxx.ll tests/xxx.jas`<br>
    For eash test case, it will generate an `xxx.ll` file which contains the llvm assembly of the source file, and an `xxx.html` file which could be opened in browser to see the token stream and the AST of the source file
