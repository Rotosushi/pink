Pink is a proof of concept programming language frontend, using llvm as it's backend.

the makefile should build on almost any linux disro,
the only requirements are:
    clang llvm libc++ (12)
    re2c (2.2)

running:

mkdir build
make

should build the program.

running:
mkdir test/build
make test

should build the tests.


Because I had to answer the question, what is the difficulty in implementing functions as values
in a C-like programming language? Because from my perspective, other than the major difference
listed above, C and a simply typed strictly evaluating Lambda Calculus are very similar languages.
