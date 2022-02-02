

#include "core/Compile.h"


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <system_error>

#include "aux/Error.h"


#include "llvm/Support/raw_ostream.h" // llvm::raw_fd_stream


namespace pink {
	
	// #TODO: Handle the case where there is more input than can fit in memory all at once,
	//			or we simply do not have all of the input available yet, like over a socket.
	std::string ReadEntireFile(std::string& input_filename)
	{
		std::string temp;
		std::string result;
		
		std::fstream infile(input_filename);
		
		if (!infile.is_open())
		{
			pink::FatalError("Could not open input file" + input_filename, __FILE__, __LINE__);
			return std::string("");
		}
		
		while (!infile.eof())
		{
			std::getline(infile, temp, '\n');
			
			result += temp;
		}
		
		infile.close();
		
		return result;
	}


	void Compile(CLIOptions& options, Environment& env)
	{
		std::string  inbuf(ReadEntireFile(options.input_file));
		
		std::error_code outfile_error;
		llvm::raw_fd_stream outfile(options.output_file, outfile_error); // using a llvm::raw_fd_stream for llvm::Module::Print
		
		if (outfile_error)
		{
			std::stringstream error_message;
			
			error_message << outfile_error;
			pink::FatalError("Could not open output file [" + options.output_file + "] because of an error: " + error_message.str(), __FILE__, __LINE__);
		}
		
		// #TODO: make the extraction method handle the case where we are parsing a file that 
		//			is too big to fit in memory, and still carry over the state of the parser 
		//			after we refill the input buffer.
		
		
		pink::Outcome<std::unique_ptr<pink::Ast>, pink::Error> term = env.parser.Parse(inbuf, env);
		
		if (!term)
		{
			// #TODO: Handle more than the first error detected in the input code.
			// #TODO: Extract the line that produced the error from the buffer and 
			// 			pass it as an argument here.
			FatalError(term.GetTwo().ToString(""), __FILE__, __LINE__);
		}
		else 
		{
			pink::Outcome<pink::Type*, pink::Error> type = term.GetOne()->Getype(env);
			
			if (!type) 
			{
				FatalError(type.GetTwo().ToString(""), __FILE__, __LINE__);
			}
			else 
			{
				// as a side effect Codegen builds all of the llvm assembly within the module using the llvm::IRBuilder<>
				pink::Outcome<llvm::Value*, pink::Error> value = term.GetOne()->Codegen(env);
				
				if (!value)
				{
					FatalError(value.GetTwo().ToString(""), __FILE__, __LINE__);
				}
			}
		}
		
		// simply print to the output file the code generated 
		env.module.print(outfile, nullptr);
		
		outfile.close();
	}

}

