

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
	
/*
 *  we can handle multiple errors by refactoring the parser 
 *  to maintain it's current position after returning from 
 *  an error, and then being able to contine parsing the 
 *  next line of code; up to some sane error limit.
 * 
 *  we can support parsing from a stream if we have a push 
 *  parser instead of a pull parser, where we don't require
 *  all of the file to be in memory simultaneously.

 *  when we define multiple file compilation, we are going to 
 *  allow use-before-definition. we can do this if we catch when 
 *  the parser returns a undefined symbol error then later 
 *  when we parse the definition/declaration of the symbol we go back 
 *  to fill in the usage using that definition. this is only going to 
 *  be done at the global declaration/definition level. because function bodies emit 
 *  code which has an implicit ordering already that I would like to preserve.
 *
 *  anyways, like with false_bindings, we could simply maintain a list of 
 *  undefined symbol usages, and then use that to directly fill in the
 *  usage when we parse the definition/declaration.
 *
 *  there are three main instances of use-before-definition
 *
 *  functions
 *
 *  types
 *
 *  globals
 *
 */	
	void Compile(std::shared_ptr<Environment> env)
	{
		std::string  inbuf(ReadEntireFile(env->options->input_file));
		
    std::string outfilename = env->options->output_file;
		std::string objoutfilename      = outfilename + ".o";
		std::string llvmoutfilename     = outfilename + ".ll";
		std::string assemblyoutfilename = outfilename + ".s";
		std::string exeoutfilename      = outfilename + ".exe";
		
		// #TODO: make the extraction method handle the case where we are parsing a file that 
		//			is too big to fit in memory, and still carry over the state of the parser 
		//			after we refill the input buffer.
		
		pink::Outcome<std::unique_ptr<pink::Ast>, pink::Error> term = env->parser->Parse(inbuf, env);
		
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
		
		// if the optimization level is greater than zero,
    // optimize the code. 
    // #TODO: look more into what would be good optimizations to run.
		if (env->options->optimization_level != llvm::OptimizationLevel::O0)
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
			
			llvm::ModulePassManager MPM = PB.buildPerModuleDefaultPipeline(env->options->optimization_level);
			
			// Run the optimizer agains the IR 
			MPM.run(*env->module, MAM);
		}
		
		// emit the module to the output file, according to the format specified
		if (env->options->emit_llvm)
		{
			std::error_code outfile_error;
			llvm::raw_fd_ostream outfile(llvmoutfilename, outfile_error); // using a llvm::raw_fd_stream for llvm::Module::Print
			
			if (outfile_error)
			{
				std::stringstream error_message;
				
				error_message << outfile_error;
				pink::FatalError("Could not open output file [" + outfilename + "] because of an error: " + error_message.str(), __FILE__, __LINE__);
			}
			
			// simply print to the output file the 
			// code generated, as it is already in llvm IR
			env->module->print(outfile, nullptr);
			
			outfile.close();
		}
		
		if (env->options->emit_assembly)
		{
		
			std::error_code outfile_error;
			llvm::raw_fd_ostream outfile(assemblyoutfilename, outfile_error); // using a llvm::raw_fd_stream for llvm::Module::Print
			
			if (outfile_error)
			{
				std::stringstream error_message;
				
				error_message << outfile_error;
				pink::FatalError("Could not open output file [" + outfilename + "] because of an error: " + error_message.str(), __FILE__, __LINE__);
			}
			
			llvm::legacy::PassManager asmPrintPass;
			if (env->target_machine->addPassesToEmitFile(asmPrintPass, outfile, nullptr, llvm::CodeGenFileType::CGFT_AssemblyFile))
			{
				FatalError("the target_machine " 
                    + env->target_machine->getTargetTriple().str() 
                    + " cannot emit an assembly file of this type",
                   __FILE__,
                   __LINE__);
			}
			
			asmPrintPass.run(*env->module);
			
			outfile.close();
		}
		
		if (env->options->emit_object)
		{	
			std::error_code outfile_error;
			llvm::raw_fd_ostream outfile(objoutfilename, outfile_error); // using a llvm::raw_fd_stream for llvm::Module::Print
			
			if (outfile_error)
			{
				std::stringstream error_message;
				
				error_message << outfile_error;
				pink::FatalError("Could not open output file [" + outfilename+ "] because of an error: " + error_message.str(), __FILE__, __LINE__);
			}
			
			llvm::legacy::PassManager objPrintPass;
			if (env->target_machine->addPassesToEmitFile(objPrintPass, outfile, nullptr, llvm::CodeGenFileType::CGFT_ObjectFile))
			{
				FatalError("the target machine " 
                    + env->target_machine->getTargetTriple().str() 
                    + " cannot emit an object file of this type",
				           __FILE__, 
                   __LINE__);
			}
			
			objPrintPass.run(*env->module);
			
			outfile.close();
		}
		
	}

}

