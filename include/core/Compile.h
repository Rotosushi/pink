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
			- Such as a really big source file, or socket transmission of a file, of the like.
			
		- How do we handle multiple input files being stitched together?
			- such as when all files are provided in one call to the compiler, 
			- or when the files are each compiled separately and then we are linking together multiple object files?
			- Is it one Environment per file? (I think so, to separate the decls from the imports)
		
		- How do we translate to different output langauges?	
		
		- I am sure there are more
		
	What we are going to do is simplify the requirements to cover as much ground as we can,
	then shore up the graps left later.
	
		We now read the file one line at a time, so we could support streaming inputs,
		but we still buffer everything that we read, and there are no provisions in case
		we exceed available memory for a given file. (yet) however one solution would be
		to buffer all possible input for a given file in the same array, and simply shift
		the input along as it's read in. this keeps the memory allocated per file constant
		for each file being processed.
		
		The second simplification is that the environment is going to be constructed for native
		code generation only at first. so we are only concerned with the native assembly format,
		the llvm intermidiate representation, and object files. all of which can be handled via

*/

/**
 * @brief The namespace for the entire project
 * 
 * \todo where is the best place to put the documentation for the whole namespace?
 * 
 */
namespace pink {
	/**
	 * @brief Runs the Compilation process over the given Environment
	 * 
	 * Broadly:
	 * 	1) Construct an Ast representing a single term of the langauge
	 * 	2) Attempt to give that term a type
	 *  	- if we *could* type the term, then add it to a list of valid terms for this compilation unit
	 * 		- if we *could not* type the term, then report the Error as the reason and end compilation.
	 *  3) Attempt to generate the code from each of the valid terms constructed by steps 1 and 2.
	 *  
	 * @param[in,out] env the environment representing the compilation unit to compile.
	 */
	void Compile(const Environment& env);
}



