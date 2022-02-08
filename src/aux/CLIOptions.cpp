#include <getopt.h>

#include "PinkConfig.h"
#include "aux/Error.h"
#include "aux/CLIOptions.h"


namespace pink {
	CLIOptions::CLIOptions(std::string infile, std::string outfile, bool verbose, bool emit_asm, bool emit_obj, bool emit_llvm, bool cannonical_llvm, llvm::OptimizationLevel optimization_level, bool link)
		: input_file(infile), output_file(outfile), verbose(verbose),
		  emit_assembly(emit_asm), emit_object(emit_obj), emit_llvm(emit_llvm),
		  cannonical_llvm(cannonical_llvm), optimization_level(optimization_level), link(link)
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
			<< "-i <arg>, --infile <arg>, --input <arg>: specifies the input source filename.\n"
			<< "-o <arg>, --outfile <arg>, --output <arg>: specifies the output filename.\n"
			<< "-O <arg>, --optimize <arg>: specifies the optimization level to use.\n\t valid arguments are:\n\t\t 0 (none),\n\t\t 1 (limited),\n\t\t 2 (regular),\n\t\t 3 (high, may affect compile times),\n\t\t s (small code size), \n\t\t z (very small code size, increases run times)\n"
			<< "-C --cannonical-llvm: Emits llvm IR in the cannonical SSA form, disabling all optimizations, this overrides any -O options\n"
			<< std::endl;
	}

	CLIOptions ParseCLIOptions(std::ostream& out, int argc, char** argv)
	{
		const char* short_options = "hvi:o:O:Llcs";
		
		std::string input_file;
		std::string output_file;
		int verbose = 0;
		bool emit_llvm = false;
		bool emit_asm  = false;
		bool emit_obj  = true;
		bool cannonical_llvm = false;
		llvm::OptimizationLevel optimization_level = 
			llvm::OptimizationLevel::O0;
		bool link = true;
		
		static struct option long_options[] = 
		{
			{"verbose",  no_argument,       &verbose, 1 }, // getopt_long returns zero in this case
			{"brief",      no_argument,       &verbose, 0 },
			{"help",       no_argument,       nullptr, 'h'},
			{"version",    no_argument,       nullptr, 'v'},
			{"infile",     required_argument, nullptr, 'i'},
			{"input",      required_argument, nullptr, 'i'},
			{"outfile",    required_argument, nullptr, 'o'},
			{"output",     required_argument, nullptr, 'o'},
			{"optimize",   required_argument, nullptr, 'O'},
			{"cannonical-llvm", no_argument,  nullptr, 'L'},
			{"emit-llvm",  no_argument,       nullptr, 'l'},
			{"emit-asm",   no_argument,       nullptr, 's'},
			{"emit-obj",   no_argument,       nullptr, 'c'},
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
				
				case 'L':
				{
					cannonical_llvm = true;
					emit_llvm = true;
					link = false;
					break;
				}
				
				case 'l':
				{
					emit_llvm = true;
					link = false;
					break;
				}
				
				case 'c':
				{
					emit_obj  = true;
					link = false;
					break;
				}
				
				case 's':
				{
					emit_asm  = true;
					link = false;
					break;
				}				
				
				case 'O':
				{
					switch(optarg[0])
					{
						case '0':
						{
							optimization_level = llvm::OptimizationLevel::O0;
							break;
						}
						
						case '1':
						{
							optimization_level = llvm::OptimizationLevel::O1;
							break;
						}
						
						case '2':
						{
							optimization_level = llvm::OptimizationLevel::O2;
							break;
						}
						
						case '3':
						{
							optimization_level = llvm::OptimizationLevel::O3;
							break;
						}
						
						case 's':
						{
							optimization_level = llvm::OptimizationLevel::Os;
							break;
						}
						
						case 'z':
						{
							optimization_level = llvm::OptimizationLevel::Oz;
							break;
						}
						
						default: 
						{
							FatalError("Option: " + std::string(optarg) + " is not a valid optimization level, use one of: 0, 1, 2, 3, s, z", __FILE__, __LINE__);
						}
					}
					break;
				}
				
				case '?': 
				{
					// the unknown option case, getopt_long printed an error message already
					break;
				}
				
				default:
				{
					FatalError("Error while parsing option: " + std::string(optarg), __FILE__, __LINE__);
					break;
				}
			}
		}
	
		if (input_file == "")
		{
			out << "input file is required." << std::endl;
			exit(0);
		}
		
		// give a default name to the output file.
		if (output_file == "")
		{
			output_file = input_file;
		}
	
		return CLIOptions(input_file, output_file, verbose == 1, emit_asm, emit_obj, emit_llvm, cannonical_llvm, optimization_level, link);
	}

}
