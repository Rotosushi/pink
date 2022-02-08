

#include "core/Compile.h"


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <system_error>

#include <stdlib.h> // system

#include "aux/Error.h"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h" // llvm::PassBuilder
#include "llvm/Analysis/AliasAnalysis.h" // llvm::AAManager
#include "llvm/Support/raw_ostream.h" // llvm::raw_fd_stream
#include "llvm/Support/raw_os_ostream.h"

#include "lld/Common/Driver.h" // lld::elf::link


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
	
	
	void Compile(CLIOptions& options, Environment& env, llvm::TargetMachine* target_machine)
	{
		std::string  inbuf(ReadEntireFile(options.input_file));
		
		std::string objoutfilename = options.output_file + ".o";
		std::string llvmoutfilename = options.output_file + ".ll";
		std::string assemblyoutfilename = options.output_file + ".s";
		std::string exeoutfilename = options.output_file + ".exe";
		
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
		
		// run the optimizations against the parsed module, if we weren't told to emit 
		// cannonical llvm IR.
		if (!options.cannonical_llvm)
		{
			// These are the analysis pass managers that run the actual 
			// analysis and optimization code against the IR.
			llvm::LoopAnalysisManager LAM;
			llvm::FunctionAnalysisManager FAM;
			llvm::CGSCCAnalysisManager CGAM;
			llvm::ModuleAnalysisManager MAM;
			
			// https://llvm.org/doxygen/classllvm_1_1PassBuilder.html
			llvm::PassBuilder PB;
		
			// #TODO: what are the default Alias Analysis that this constructs?
			FAM.registerPass([&]{ return PB.buildDefaultAAPipeline(); });
			
			// Register the AnalysisManagers with the Pass Builder
			PB.registerModuleAnalyses(MAM);
			PB.registerCGSCCAnalyses(CGAM);
			PB.registerFunctionAnalyses(FAM);
			PB.registerLoopAnalyses(LAM);
			// This registers each of the passes with eachother, so they 
			// can perform optimizations together, lazily
			PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);
			
			// #TODO: select optimization level based upon a flag in the CLIOptions.
			llvm::ModulePassManager MPM = PB.buildPerModuleDefaultPipeline(options.optimization_level);
			
			// Run the optimizer agains the IR 
			MPM.run(env.module, MAM);
		}
		
		// emit the module to the output file, according to the format specified
		if (options.emit_llvm)
		{
			
		
			std::error_code outfile_error;
			llvm::raw_fd_ostream outfile(llvmoutfilename, outfile_error); // using a llvm::raw_fd_stream for llvm::Module::Print
			
			if (outfile_error)
			{
				std::stringstream error_message;
				
				error_message << outfile_error;
				pink::FatalError("Could not open output file [" + options.output_file + "] because of an error: " + error_message.str(), __FILE__, __LINE__);
			}
			
			// simply print to the output file the 
			// code generated, as it is already in llvm IR
			env.module.print(outfile, nullptr);
			
			outfile.close();
		}
		
		if (options.emit_assembly)
		{
		
			std::error_code outfile_error;
			llvm::raw_fd_ostream outfile(assemblyoutfilename, outfile_error); // using a llvm::raw_fd_stream for llvm::Module::Print
			
			if (outfile_error)
			{
				std::stringstream error_message;
				
				error_message << outfile_error;
				pink::FatalError("Could not open output file [" + options.output_file + "] because of an error: " + error_message.str(), __FILE__, __LINE__);
			}
			
			llvm::legacy::PassManager asmPrintPass;
			if (target_machine->addPassesToEmitFile(asmPrintPass, outfile, nullptr, llvm::CodeGenFileType::CGFT_AssemblyFile))
			{
				FatalError("the target_machine cannot emit an assembly file of this type",
				__FILE__, __LINE__);
			}
			
			asmPrintPass.run(env.module);
			
			outfile.close();
		}
		
		if (options.emit_object)
		{	
			std::error_code outfile_error;
			llvm::raw_fd_ostream outfile(objoutfilename, outfile_error); // using a llvm::raw_fd_stream for llvm::Module::Print
			
			if (outfile_error)
			{
				std::stringstream error_message;
				
				error_message << outfile_error;
				pink::FatalError("Could not open output file [" + options.output_file + "] because of an error: " + error_message.str(), __FILE__, __LINE__);
			}
			
			llvm::legacy::PassManager objPrintPass;
			if (target_machine->addPassesToEmitFile(objPrintPass, outfile, nullptr, llvm::CodeGenFileType::CGFT_ObjectFile))
			{
				FatalError("the target machine cannot emit an object file of this type",
				__FILE__, __LINE__);
			}
			
			objPrintPass.run(env.module);
			
			outfile.close();
		}
		
		if (options.emit_object && options.link)
		{
			// linking step
			llvm::raw_os_ostream std_err(std::cerr);
			llvm::raw_os_ostream std_out(std::cout);
			
			//#TODO find these files in a more dynamic way
			std::string crt1 = "/usr/lib/x86_64-linux-gnu/crt1.o";
			std::string crti = "/usr/lib/x86_64-linux-gnu/crti.o";
			std::string crtbegin = "/usr/lib/gcc/x86_64-linux-gnu/11/crtbegin.o";
			std::string crt1path = "-L/usr/lib/x86_64-linux-gnu";
			std::string crtbeginpath = "-L/usr/lib/gcc/x86_64-linux-gnu/11";
			std::string crtend = "/usr/lib/gcc/x86_64-linux-gnu/11/crtend.o";
			std::string crtn   = "/usr/lib/x86_64-linux-gnu/crtn.o";
			
			std::vector<const char *> lld_args(
				{"ld.lld-14",
				 "-m", "elf_x86_64",
				 //"-dynamic-linker", "lib64/ld-linux-x86-64.so.2",
				 crt1.data(),
				 crti.data(),
				 crtbegin.data(),
				 crt1path.data(),
				 crtbeginpath.data(),
				 "/usr/lib/x86_64-linux-gnu/libc.so",
				 objoutfilename.data(),
				 "-o", exeoutfilename.data(),
				 crtend.data(),
				 crtn.data()});
			
			lld::elf::link(lld_args, std_out, std_err, /* exitEarly */ false, /* disableOutput */ false);
			
			//std::string command = "ld.lld-14 " + options.input_file + ".o -o " + options.output_file + ".exe";
			//int result = std::system(command.data());
		}
	}

}

