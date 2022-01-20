Pink is a proof of concept programming language frontend, using llvm as it's backend.

For my Grandfather (1944 - 2022)

the makefile should build on almost any linux disro,
the only requirements are:
    clang llvm libstdc++ (12)
    re2c (2.2)

running:

mkdir build &&
make

builds the program.

running:
mkdir test/build &&
make test

builds the tests.


A cmake file has been added which should also build the program,

mkdir build &&
cd build  &&
cmake .. &&
cmake --build . 

will build the program and the tests currently.


Because I had to answer the question, what is the difficulty in implementing functions as values
in a C-like programming language? Because from my perspective, other than the major difference
listed above, C and a simply typed strictly evaluating Lambda Calculus are very similar languages.
