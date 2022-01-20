
#include <iostream>

#include "PinkConfig.h"

/*
	In order to handle program options like --version or -v, etc...
	
	The easiest thing to do would be to include "getopt.h"
	and use getopt_long(...) to parse the options provided 
	to the program.
	
	This works fantastically on unix like operating systems, and 
	this header file should be available on almost any conceivable 
	POSIX operating system. Which shouldn't limit this programs 
	distributability within the wide array of flavors of Linux.
	
	However, getopt.h is not available on Windows, and I have no idea 
	of it's availability on Apple Computers. So if we want this 
	compiler to be available cross platform, using getopt might not 
	facilitate that directly. On windows in particular there are several 
	workarounds, like MinGW/CygWin (whichever is still supported) or the 
	Windows Subsystem for Linux, which runs a full linux OS within the 
	host windows system. and again, for Apple devices I have no idea
	what is actually available.
	
	So, since this is a small project of just me so far, releasing only 
	on Linux to begin seems like a fine option.
	Thus, using getopt_long seems like a perfectly reasonable solution
	to this particular issue for the time being. 
	Noting down that this might be a hiccup in the future also seems like 
	a reasonable compromise solution to get something working and usable 
	by others while	I am working by myself.
	
	And in that line of thinking, what are some good options to have/consider 
	for a 'complete' compiler? Obviously the usual suspects of 
	
	-h or --help, -v or --version, --verbose, -i or --input, -o or --output, etc...
	
	and it would also seem prudent to provide things that control 
	how llvm is used by the program.
	
	--emit-llvm-assembly, --optimization-level ..., --use-linker ..., etc..
	
	
*/

int main(int argc, char** argv)
{
	// report version #TODO: Only do this when supplied with 
	// the argument --version 
	
	std::cout << argv[0] << " Version " << pink_VERSION_MAJOR 
			  << "." << pink_VERSION_MINOR << std::endl; 	

    return 0;
}
