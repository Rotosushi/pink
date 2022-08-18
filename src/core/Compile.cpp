

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
#include "llvm/Support/FileSystem.h"


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
		//std::string  inbuf(ReadEntireFile(env->options->input_file));
	  std::fstream infile;
    std::string outfilename = env->options->output_file;
		std::string objoutfilename      = outfilename + ".o";
		std::string llvmoutfilename     = outfilename + ".ll";
		std::string assemblyoutfilename = outfilename + ".s";
		std::string exeoutfilename      = outfilename + ".exe";

    infile.open(env->options->input_file);
    
    if (!infile.is_open())
    {
      FatalError("Could not open input file " + env->options->input_file, __FILE__, __LINE__);
    }

    env->parser->SetIStream(&infile);

    std::vector<std::unique_ptr<pink::Ast>> valid_terms;
   
    while (!env->parser->EndOfInput())
    { 
      pink::Outcome<std::unique_ptr<pink::Ast>, pink::Error> term = env->parser->Parse(env);
      
      if (!term)
      {
        pink::Error error(term.GetTwo());
        
        if (error.code == Error::Code::EndOfFile)
        {
          // if we didn't parse any terms, then there is nothing to optimize,
          // emit, or link together thus we are safe to simply exit.
          if (valid_terms.size() == 0)
          {
            FatalError("Warning: parsed an empty source file " + env->options->input_file, __FILE__, __LINE__);
          }
          // else we did parse some terms and simply reached the end of the
          // source file, so we can safely continue with compilation of the 
          // valid terms.
          break; 
        }
        else
        {
          // #TODO: Handle more than the first error detected in the input code.
          // #TODO: Extract the line that produced the error from the buffer and 
          // 			pass it as an argument here.
          FatalError(error.ToString(""), __FILE__, __LINE__);
        }
      }
      else 
      {
        pink::Outcome<pink::Type*, pink::Error> type = term.GetOne()->Getype(env);
       
        // if not type and error == use-before-definition
        // {
        //   push term into a llvm::DenseMap<pink::InternedString, std::pair<std::unique_ptr<pink::Ast>, std::vector<std::unique_ptr<pink::Ast>>::iterator>>
        //   // where the InternedString holds the variable name which was used
        //   // before it was defined, and std::unique_ptr<pink::Ast> holds the
        //   // term which failed to type. and the std::vector<...>::iterator
        //   // holds the place where we would have inserted this term into the 
        //   // original vector. that place where we would have inserted is
        //   // instead filled with a dummy term later, which is intended to be
        //   // replaced by the real term once there are no use-before-definition 
        //   // errors.
        //   // 
        // } 
        if (!type) 
        {
          FatalError(type.GetTwo().ToString(""), __FILE__, __LINE__);
        }
        else 
        {
          valid_terms.push_back(std::move(term.GetOne()));
        }
      }
    }
	
    // if there are no invalid terms when we get here.
    // that is, if every bit of source we parsed typechecks
    // then we simply have to emit all of their definitions into
    // the module. (perhaps this too will require a 
    // use-before-definition buffer?)
    // however, we have one thing to take care of, after typing all 
    // expressions, the Bind form will have constructed false bindings 
    // to allow the type of bound names to be used in later expressions.
    // so, we have to unbind each name that was falsely bound,
    // so that Codegen can bind those names to their actual types.
    for (InternedString fbnd : env->false_bindings)
    {
      env->bindings->Unbind(fbnd);
    }
    // then since we unbound them, clear the names from the list
    env->false_bindings.clear();

    
    for (std::unique_ptr<pink::Ast>& term : valid_terms)
    {
      // as a side effect Codegen builds all of the llvm assembly within the module using the llvm::IRBuilder<>
  		pink::Outcome<llvm::Value*, pink::Error> value = term->Codegen(env);

		  if (!value)
		  {
			  FatalError(value.GetTwo().ToString(""), __FILE__, __LINE__);
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
      llvm::SmallVector<char> buf;
	    llvm::raw_svector_ostream outstream(buf);

			std::error_code outfile_error;
			llvm::raw_fd_ostream outfile(assemblyoutfilename, outfile_error); // using a llvm::raw_fd_stream for llvm::Module::Print
			
			if (outfile_error)
			{
				std::stringstream error_message;
				
				error_message << outfile_error;
				pink::FatalError("Could not open output file [" + outfilename + "] because of an error: " + error_message.str(), __FILE__, __LINE__);
			}
			
			llvm::legacy::PassManager asmPrintPass;
      // addPassesToEmitFile(legacy::PassManager pass_manager,
      //                     llvm::raw_fd_ostream& outfile,
      //                     llvm::raw_fd_ostream* dwarfOutfile,
      //                     llvm::CodeGenFileType CodeGenFileType,)
      //
      // notice the nullptr will need to be replaced with a pointer to the 
      // dwarf object output file, once we emit debugging information.
			if (env->target_machine->addPassesToEmitFile(asmPrintPass, outstream, nullptr, llvm::CodeGenFileType::CGFT_AssemblyFile))
			{
				FatalError("the target_machine " 
                    + env->target_machine->getTargetTriple().str() 
                    + " cannot emit an assembly file of this type",
                   __FILE__,
                   __LINE__);
			}
			
			asmPrintPass.run(*env->module);
		
      
		  outfile << outstream.str();

      if (outfile.has_error())
      {
        std::stringstream errmsg;
        errmsg << outfile.error();
        pink::FatalError("The output stream encountered an error: "
                          + errmsg.str(),
                          __FILE__,
                          __LINE__);
      }  
      else 
      {
       outfile.close(); 
		  }
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

