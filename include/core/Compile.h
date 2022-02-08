#pragma once 

#include "aux/CLIOptions.h"
#include "aux/Environment.h"
#include "llvm/Target/TargetMachine.h"

/*
	Handle the Translation of a file into an output format.
	(right now that just means llvm assembly)
	
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
	
		The first simplification is that we will simply buffer the entire input file in one go,
		the start the process of translation. If the file is too big, that will be an error, for now.
		
		The second simplification is that the environment is going to be constructed for native
		code generation only at first.
*/

namespace pink {

void Compile(CLIOptions& options, Environment& env, llvm::TargetMachine* target_machine);

}



