#pragma once 

#include "aux/CLIOptions.h"
#include "aux/Environment.h"
#include "llvm/Target/TargetMachine.h"

/*
	Handle the Translation of a file into the specified output format.
	(we support llvm IR, native assembly (x86-64), and object files)

	There are quite a few considerations here:
		- What if the file is too big to fit into memory all at once?
			- Such as a really big source file, or socket transmission of a file, of the like.
			
		- How do we handle multiple input files being stitched together?
			- such as when all files are provided in one call to the compiler, 
			- or when the files are each compiled separately and then we are linking together multiple object files?
			- Is it one Environment per file? (I think so, to separate the decls from the imports)
		
		- How do we translate to different output langauges?	
		
		- I am sure there are more, but that is what i can see right now.
		
	What we are going to do is simplify the requirements to cover as much ground as we can,
	then shore up the graps left later.
	
		We now read the file one line at a time, so we could support streaming inputs,
		but we still buffer everything that we read, and there are no provisions in case
		we exceed available memory for a given file. (yet)
		
		The second simplification is that the environment is going to be constructed for native
		code generation only at first.
*/

namespace pink {
  void Compile(const Environment& env);
}



