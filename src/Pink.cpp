
#include "aux/CLIOptions.h"
#include "aux/Environment.h"

#include "core/Compile.h"

#include "llvm/Support/Host.h" // llvm::sys::getProcessTriple()
#include "llvm/Support/TargetRegistry.h" // llvm::TargetRegistry::lookupTarget();
#include "llvm/Support/TargetSelect.h"


#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetMachine.h"


int main(int argc, char** argv)
{
	// Parse the command line options
	pink::CLIOptions options = pink::ParseCLIOptions(std::cout, argc, argv);
	
	// Set up the default environment.
	// #TODO: figure out how to make this a bit cleaner from the perspective of main itself.
	// some function that would let us say 
	// Environment env(NativeEnvironment(CLIOptions));
	// or something similar. (i like that the environment only holds references,
	// from the perspective of typing functions, as a function builds 
	// a new environment to pass in while typing the body. and the environment 
	// is cheaper to construct if it is only references, however, we now must 
	// hold all of the members somewhere in memory, which happens to be in the 
	// main subroutine right now, but if we want to translate multiple files 
	// simultaneously, that will need multiple environments, one per file. (I think)
	// in this case, it becomes a bit more cumbersome to define multiple environments 
	// within a single scope. So defining a function that would alleviate this 
	// somewhat would be awesome. dynamic allocations of each member seems like a 
	// great start.
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
                 
    // Call the main driver code         
	Compile(options, env);

    return 0;
}
