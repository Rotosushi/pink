#pragma once 
#include <iostream>

/*
	Similar to how we use constexprs in the test suite 
	we could store all of the flags that the program uses 
	in a size_t and then we can use GNU getopt_long or 
	getopt_long_only to parse the options provided to 
	the program.
	
	from man getopt(3) 
	the layout of the options description structure for the 
	program would look something like this. 
	
	static struct option long_options[] = {
                   {"add",     required_argument, 0,  0 },
                   {"append",  no_argument,       0,  0 },
                   {"delete",  required_argument, 0,  0 },
                   {"verbose", no_argument,       0,  0 },
                   {"create",  required_argument, 0, 'c'},
                   {"file",    required_argument, 0,  0 },
                   {0,         0,                 0,  0 }
               };
               
    However, the last two arguments would be different.
    the zero positioned directly after the 'required_argument'
    or 'no_argument' is a pointer like variable which can either 
    be a nullptr, in which case when the corresponding option is 
    seen, getopt will return the value held in the next member of 
    the struct. This would work. Although it would also work to 
    store the address of an int flags variable which represented 
    the seen flags, then the values that would be returned are 
    instead written directly to that variable. This would also work.
    
    either way we still need to do some extra work to gather up any 
    arguments to the presented options.
    
    we might define a class which is constructed by ParseCLIOptions,
    which stores all of the data retrieved from the options themselves.
    THis class could also house the code for printing the help and version 
    information, and then we can use this class throughout the rest of the 
    compiler, whenever configuration information that can be influenced 
    from the command line is needed.
*/

constexpr auto CLIOPT_HELP    = 0x0001;
constexpr auto CLIOPT_VERSION = 0x0002;


size_t ParseCLIOptions(int argc, char** argv);

void PrintVersion(std::ostream& out);
void PrintHelp(std::ostream& out);
