
#include <iostream>

#include "PinkConfig.h"



int main(int argc, char** argv)
{
	// report version #TODO: Only do this when supplied with 
	// the argument --version 
	
	std::cout << argv[0] << " Version " << pink_VERSION_MAJOR 
			  << "." << pink_VERSION_MINOR << std::endl; 	

    return 0;
}
