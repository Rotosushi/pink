
#include <iostream>

#include "aux/CLIOptions.h"

int main(int argc, char** argv)
{
	pink::CLIOptions options = pink::ParseCLIOptions(std::cout, argc, argv);

    return 0;
}
