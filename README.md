Pink is a Programming Language

A cmake file builds the program and the tests.

Dependencies:
re2c needs to be available on the PATH
llvm 16 needs to be available for CMake find_package
Catch2 3 needs to be available for CMake find_package

a basic build looks like
mkdir build &&
cmake -S . -B build &&
cmake --build build




