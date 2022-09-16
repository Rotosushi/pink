

#include "aux/Environment.h"

#include "kernel/BinopPrimitives.h"
#include "kernel/UnopPrimitives.h"

#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"

#include "llvm/MC/TargetRegistry.h"

#include "llvm/Target/TargetMachine.h"

#include "llvm/Passes/OptimizationLevel.h"

namespace pink {
    Environment::Environment(
      std::shared_ptr<Flags>                       flags,
      std::shared_ptr<CLIOptions>                  options,
      std::shared_ptr<Parser>                      parser,
      std::shared_ptr<StringInterner>              symbols,
      std::shared_ptr<StringInterner>              operators,
      std::shared_ptr<TypeInterner>                types,
      std::shared_ptr<SymbolTable>                 bindings,
      std::shared_ptr<BinopTable>                  binops,
      std::shared_ptr<UnopTable>                   unops,
      std::shared_ptr<llvm::LLVMContext>           context,
      std::shared_ptr<llvm::Module>                module,
      std::shared_ptr<llvm::IRBuilder<>>           instruction_builder,
      //std::shared_ptr<llvm::DIBuilder>             debug_builder,
      const llvm::Target*                          target,
      llvm::TargetMachine*                         target_machine,
      const llvm::DataLayout                       data_layout
      )
        : false_bindings(std::make_shared<std::vector<InternedString>>()),
          flags(flags),
          options(options),
          parser(parser),
          symbols(symbols),
          operators(operators),
          types(types),
          bindings(bindings),
          binops(binops),
          unops(unops),
          context(context),
          module(module),
          instruction_builder(instruction_builder),
          //debug_builder(debug_builder),
          target(target),
          target_machine(target_machine), 
          data_layout(data_layout),
          current_function(nullptr)
    {

    }
    
    Environment::Environment(
      const Environment&           env,
      std::shared_ptr<SymbolTable> bindings
    )
    	: false_bindings(std::make_shared<std::vector<InternedString>>()),
        flags(env.flags),
        options(env.options),
        parser(env.parser),
        symbols(env.symbols),
        operators(env.operators),
    	  types(env.types),
        bindings(bindings),
        binops(env.binops),
        unops(env.unops),
        context(env.context),
        module(env.module),
        instruction_builder(env.instruction_builder),
        //debug_builder(env.debug_builder),
        target(env.target),
    	  target_machine(env.target_machine),
        data_layout(env.data_layout),
    	  current_function(env.current_function)
    {
    
    }
    
    Environment::Environment(
      const Environment&                 env,
      std::shared_ptr<SymbolTable>       bindings,
      std::shared_ptr<llvm::IRBuilder<>> instruction_builder,
      llvm::Function*                    current_function
    )
    	: false_bindings(std::make_shared<std::vector<InternedString>>()),
        flags(env.flags),
        options(env.options),
        parser(env.parser), 
        symbols(env.symbols),
        operators(env.operators),
    	  types(env.types),
        bindings(bindings), 
        binops(env.binops),
        unops(env.unops),
    	  context(env.context),
        module(env.module),
        instruction_builder(instruction_builder),
        //debug_builder(env.debug_builder),
        target(env.target),
    	  target_machine(env.target_machine),
        data_layout(env.data_layout),
    	  current_function(current_function)
    {
    
    }

    std::unique_ptr<Environment> NewGlobalEnv(std::shared_ptr<CLIOptions> options)
    {
      return NewGlobalEnv(options, &std::cin);
    }

    std::unique_ptr<Environment> NewGlobalEnv(std::shared_ptr<CLIOptions> options, std::istream* instream)
    {
      auto flags     = std::make_shared<Flags>();
      auto parser    = std::make_shared<Parser>(instream);
      auto symbols   = std::make_shared<StringInterner>();
      auto operators = std::make_shared<StringInterner>();
      auto types     = std::make_shared<TypeInterner>();
      auto bindings  = std::make_shared<SymbolTable>();
      auto binops    = std::make_shared<BinopTable>();
      auto unops     = std::make_shared<UnopTable>();
     
      std::shared_ptr<llvm::LLVMContext> context = std::make_shared<llvm::LLVMContext>();
      
      // This to me would seem to work for native code generation.
      // because the compiler is a process, and as such is running 
      // on the host machine. so the processTriple would be a 
      // target triple suitable to generating code for the host machine.
      // that is, native code generation.
      // then cross platform codegeneration would be specified via the 
      // command line, and as such we can write a switch over known and 
      // supported generation targets to select the correct target_triple.
      // (and cpu, cpu features, and any other target specific information.)
      std::string target_triple = llvm::sys::getProcessTriple();
      
      std::string error;
      const llvm::Target* target = llvm::TargetRegistry::lookupTarget(target_triple, error);

      if (target == nullptr)
      {
        FatalError(error.data(), __FILE__, __LINE__);
      }

      // get the native CPU name.
      std::string cpu = llvm::sys::getHostCPUName().str(); 
      
      // get the native CPU features.
      std::string cpu_features;
      llvm::StringMap<bool> features;

      if (llvm::sys::getHostCPUFeatures(features))
      {
        llvm::StringMap<bool>::iterator i = features.begin();
        llvm::StringMap<bool>::iterator e = features.end();
        unsigned numElems = features.getNumItems();
        unsigned j = 0;        

        while (i != e)
        {
          // the StringMap<bool> maps strings (keys) to values (bools)
          // where the bool is true if the feature is available on the target
          // and the bool is false if the feature is unavailable on the target.
          // each feature available (and specified within the feature string)
          // must be prefixed with '+', and similarly unavailable features must
          // be prefixed with '-'
          if ((*i).getValue())
          {
            cpu_features += std::string("+") + (*i).getKeyData();   
          }
          else
          {
            cpu_features += std::string("-") + (*i).getKeyData();
          }
          
          if (j < (numElems - 1))
          {
            cpu_features += ",";
          }
  
          i++;
          j++;
        }
      }
      
      llvm::TargetOptions target_options; // #TODO: this works fine default constructed, but 
                                          // reading the docs, and the class definition, this
                                          // structure seems like it is used for
                                          // optimization/profiling and specifying extra rules 
                                          // for compilation, like what floating point rules to
                                          // use, and such.
      // #TODO: position independant code is a fine default,
      // however we should allow the user to change this via the command line
      llvm::Reloc::Model code_relocation_model = llvm::Reloc::Model::PIC_;
      // #TODO: the Small x86-64 code model is a fine default for x86-64,
      // however this must change if the code being compiled becomes larger 
      // than 2GB, or if the data within the code or being processed by the 
      // code becomes larger than 2GB. So we should add an option on the 
      // command line to select between models. 
      // NOTE: this is something to look at for the future, does the 
      // codegeneration fail if we don't meet the uder 2GB requirement?
      // would our program be able to recover from such an error?
      llvm::CodeModel::Model code_model = llvm::CodeModel::Model::Small;

      llvm::TargetMachine* target_machine = nullptr;
      target_machine = target->createTargetMachine(
        target_triple,
        cpu,
        cpu_features,
        target_options,
        code_relocation_model,
        code_model
        );

      llvm::DataLayout data_layout = target_machine->createDataLayout();

      std::shared_ptr<llvm::IRBuilder<>> instruction_builder = std::make_shared<llvm::IRBuilder<>>(*context);
      std::shared_ptr<llvm::Module> module = std::make_shared<llvm::Module>(options->input_file, *context);

      module->setSourceFileName(options->input_file);
      module->setDataLayout(data_layout);
      module->setTargetTriple(target_triple);
      
      // construct classes which are used to generate debugging information
      // within the program.

      // a minor gripe about the DIBuilder class, the class itself takes
      // as an argument a pointer to a DICompileUnit, which makes some 
      // sense to associate the Builder of debug info with a class 
      // which holds data common to debugging information, the DICompileUnit.
      // the issue is that DIBuilder is also the class with a method to 
      // construct a DICompileUnit from arguments. yet no way to set the constructed 
      // compile unit as the current on associated with the class.
      // so it seems that the most natural way of constructing these two classes
      // is to create the CompileUnit and then the DIBuilder, so then why 
      // does DIBuilder hold a method for constructing a DICompileUnit?
      //

      // okay, so a somewhat minor thing to note is that if this language can
      // actually support functions as values then we will run into issues telling
      // the debugger that our language conforms to the C calling convention.
      // This is unfortunate as it m
      //const llvm::DIBuilder temp_debug_builder(*module); 
      //llvm::DICompileUnit* debug_compile_unit = temp_debug_builder->createCompileUnit(llvm::dwarf::DW_LANG_C);

      //std::shared_ptr<llvm::DIBuilder> debug_builder = std::make_shared<llvm::DIBuilder>(*module, /* allowUnresolved = */ true, debug_compile_unit);


      auto env = std::make_unique<Environment>(
          flags,
          options,
          parser,
          symbols,
          operators,
          types,
          bindings,
          binops,
          unops,
          context,
          module,
          instruction_builder,
          target,
          target_machine,
          data_layout
          );

      InitializeBinopPrimitives(*env);
      InitializeUnopPrimitives(*env);

      return env;
    }
}
