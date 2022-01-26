#include <getopt.h>

#include "PinkConfig.h"
#include "aux/Error.h"
#include "aux/CLIOptions.h"


namespace pink {
	CLIOptions::CLIOptions(std::string infile, std::string outfile, bool verbose)
		: input_file(infile), output_file(outfile), verbose(verbose)
	{
	
	}
	
	void PrintVersion(std::ostream& out)
	{
		out << "pink version: " << pink_VERSION_MAJOR 
		    << "." << pink_VERSION_MINOR << std::endl; 
	}
	
	void PrintHelp(std::ostream& out)
	{
		out << "pink Usage: \n" 
			<< "General program options: \n"
			<< "-h, --help: print this help message and exit.\n"
			<< "-v, --version: print version information and exit.\n"
			<< "--verbose: set verbose program output (defaults to brief output).\n"
			<< "--brief: reset verbose program output (this is the default behavior).\n"
			<< "-i, --infile, --input: specifies the input source filename.\n"
			<< "-o, --outfile, --output: specifies the output filename.\n"
			<< std::endl;
	}

	CLIOptions ParseCLIOptions(std::ostream& out, int argc, char** argv)
	{
		const char* short_options = "hvi:o:";
		
		std::string input_file;
		std::string output_file;
		int verbose = 0;
		
		static struct option long_options[] = 
		{
			{"verbose", no_argument,       &verbose, 1 }, // getopt_long returns zero in this case
			{"brief",   no_argument,       &verbose, 0 },
			{"help",    no_argument,       nullptr, 'h'},
			{"version", no_argument,       nullptr, 'v'},
			{"infile",  required_argument, nullptr, 'i'},
			{"input",   required_argument, nullptr, 'i'},
			{"outfile", required_argument, nullptr, 'o'},
			{"output",  required_argument, nullptr, 'o'},
			{0,         0,           0,       0}
		};
		
		int option = 0;
		int optind = 0;
		
		while (true)
		{
			option = getopt_long(argc, argv, short_options, long_options, &optind);
			
			if (option == -1)
				break; // end of options.
			
			switch(option)
			{
				case 0:
				{
					// this option set a flag, so has already been handled
					break;
				}
				
				case 'h':
				{
					PrintHelp(out);
					exit(0);
					break;
				}
				
				case 'v':
				{
					PrintVersion(out);
					exit(0);
					break;
				}
				
				case 'i':
				{
					input_file = optarg;
					break;
				}
				
				case 'o':
				{
					output_file = optarg;
					break;
				}
				
				case '?': 
				{
					// the unknown option case, getopt_long printed an error message already
					break;
				}
				
				default:
				{
					FatalError("Error while parsing options", __FILE__, __LINE__);
					break;
				}
			}
		}
	
		return CLIOptions(input_file, output_file, verbose == 1);
	}

}
