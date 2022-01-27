
#include <iostream>

#include "aux/CLIOptions.h"

#include <fstream>
#include <string>
#include <sstream>
#include <system_error>
#include "aux/Error.h"
#include "aux/Environment.h"

#include "llvm/Support/Host.h" // llvm::sys::getProcessTriple()
#include "llvm/Support/TargetRegistry.h" // llvm::TargetRegistry::lookupTarget();
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h" // llvm::raw_fd_stream

#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetMachine.h"

int main(int argc, char** argv)
{
	pink::CLIOptions options = pink::ParseCLIOptions(std::cout, argc, argv);
	
	/*
		TODO: Refactor this into something better. (REPL or Multiline file input)
	*/
	pink::Parser         parser;
    pink::StringInterner symbols;
    pink::StringInterner operators;
    pink::TypeInterner   types;
    pink::SymbolTable    bindings;
    pink::BinopTable     binops;
    pink::UnopTable      unops;

    llvm::LLVMContext context;
    llvm::IRBuilder<> builder(context);

    //llvm::InitializeAllTargetInfos();
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetDisassembler();


    std::string target_triple = llvm::sys::getProcessTriple();

    std::string error;
    const llvm::Target* target = llvm::TargetRegistry::lookupTarget(target_triple, error);

    if (!target)
    {
        pink::FatalError(error.data(), __FILE__, __LINE__);
        return 1;
    }

    std::string cpu = "x86-64";
    std::string cpu_features = "";
    llvm::TargetOptions target_options;
    llvm::Reloc::Model crm = llvm::Reloc::Model::PIC_;
    llvm::CodeModel::Model code_model = llvm::CodeModel::Model::Small;
    llvm::CodeGenOpt::Level opt_level = llvm::CodeGenOpt::Level::None;

    llvm::TargetMachine* target_machine = target->createTargetMachine(target_triple,
                                                                      cpu,
                                                                      cpu_features,
                                                                      target_options,
                                                                      crm,
                                                                      code_model,
                                                                      opt_level);

    llvm::DataLayout data_layout(target_machine->createDataLayout());
    llvm::Module      module(options.input_file, context);


    pink::Environment env(parser, symbols, operators, types, bindings, binops, unops,
                          target_triple, data_layout, context, module, builder);
                          
	std::fstream infile;
	std::string  inbuf;
	
	infile.open(options.input_file);

	if (!infile.is_open())
	{
		pink::FatalError("Could not open input file" + options.input_file, __FILE__, __LINE__);
		return 1;
	}
	
	std::error_code outfile_error;
	llvm::raw_fd_stream outfile(options.output_file, outfile_error); // using a llvm::raw_fd_stream for llvm::Module::Print
	
	if (outfile_error)
	{
		std::stringstream error_message;
		
		error_message << outfile_error;
		pink::FatalError("Could not open output file [" + options.output_file + "] because of an error: " + error_message.str(), __FILE__, __LINE__);
		return 1;
	}

	std::getline(infile, inbuf, '\n');
	
	pink::Outcome<std::unique_ptr<pink::Ast>, pink::Error> term = env.parser.Parse(inbuf, env);
	
	if (!term)
	{
		term.GetTwo().Print(std::cout, inbuf);
	}
	else 
	{
		pink::Outcome<pink::Type*, pink::Error> type = term.GetOne()->Getype(env);
		
		if (!type) 
		{
			type.GetTwo().Print(std::cout, inbuf);
		}
		else 
		{
			pink::Outcome<llvm::Value*, pink::Error> value = term.GetOne()->Codegen(env);
			
			if (!value)
			{
				value.GetTwo().Print(std::cout, inbuf);
			}
			else 
			{
				// simply print out what code was generated,
				// #TODO: make this part more robust, handling some amount of errors before aborting.
				env.module.print(outfile, nullptr);
			}
		}
	}
	
	infile.close();
	outfile.close();

    return 0;
}
