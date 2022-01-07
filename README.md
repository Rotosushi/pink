Pink is a proof of concept programming language frontend, using llvm as it's backend.

the makefile should build on almost any linux disro,
the only requirements are:
    clang llvm libc++ re2c

running:

mkdir build
mkdir test/build
make

should build the program.

running:
make test

should build the tests.


Because I had to answer the question, what is the difficulty in implementing functions as values
in a c like programming language? Because from my perspective other than the major difference
listed above, C, and a simply typed, strictly evaluating, Lambda Calculus
 are very similar languages.


 I hope to get this project to a working stable version soon.
 Though, that doesn't mean 'fully featured programming language,'
 at least, not for the first stable version.
