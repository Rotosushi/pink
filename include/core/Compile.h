/**
 * @file Compile.h
 * @brief Header for the function Compile
 * @version 0.1
 *
 */
#pragma once

#include "aux/Environment.h"

/*
  #NOTE: 9/16/2022
  Handle the Translation of a file into the specified output format.
  (we support llvm IR, native assembly (x86-64), and object files)

  There are quite a few considerations here:
  - What if the file is too big to fit into memory all at once?
    - Such as a really big source file, or socket
      transmission of a file, of the like.

  - How do we handle multiple input files being stitched together?
    - such as when all files are provided in one call to the
      compiler,
    - or when the files are each compiled separately and
      then we are linking together multiple object files?
    - Is it one Environment per file? (I think so, to
      separate the decls from the imports)

  - How do we translate to different output langauges?

  - I am sure there are more

  What we are going to do is simplify the requirements to cover as much
   ground as we can, then shore up the graps left later.

*/

/**
 * @brief The namespace for the entire project
 *
 * \todo where is the best place to put the documentation for the whole
 * namespace?
 *
 */
namespace pink {
/**
 * @brief Compile the source file represented by the given
 * Environment.
 *
 * @param argc
 * @param argv
 */
auto Compile(std::ostream &err, Environment &env) -> int;
} // namespace pink
