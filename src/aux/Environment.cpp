

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
      std::shared_ptr<llvm::IRBuilder<>>           builder,
      llvm::TargetMachine*                         target_machine,
      const llvm::DataLayout                       data_layout
      )
        : false_bindings(std::vector<InternedString>()),
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
          builder(builder),
          target_machine(target_machine), 
          data_layout(data_layout),
          current_function(nullptr)
    {

    }
    
    Environment::Environment(
      std::shared_ptr<Environment> env,
      std::shared_ptr<SymbolTable> bindings
    )
    	: flags(env->flags),
        options(env->options),
        parser(env->parser),
        symbols(env->symbols),
        operators(env->operators),
    	  types(env->types),
        bindings(bindings),
        binops(env->binops),
        unops(env->unops),
        context(env->context),
        module(env->module),
        builder(env->builder),
    	  target_machine(env->target_machine),
        data_layout(env->data_layout),
    	  current_function(env->current_function)
    {
    
    }
    
    Environment::Environment(
      std::shared_ptr<Environment>       env,
      std::shared_ptr<SymbolTable>       bindings,
      std::shared_ptr<llvm::IRBuilder<>> builder,
      llvm::Function*                    current_function
    )
    	: flags(env->flags),
        options(env->options),
        parser(env->parser), 
        symbols(env->symbols),
        operators(env->operators),
    	  types(env->types),
        bindings(bindings), 
        binops(env->binops),
        unops(env->unops),
    	  context(env->context),
        module(env->module),
        builder(builder),
    	  target_machine(env->target_machine),
        data_layout(env->data_layout),
    	  current_function(current_function)
    {
    
    }

    std::shared_ptr<Environment> NewGlobalEnv(std::shared_ptr<CLIOptions> options)
    {
      return NewGlobalEnv(options, nullptr);
    }

    std::shared_ptr<Environment> NewGlobalEnv(std::shared_ptr<CLIOptions> options, std::istream* instream)
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
      // #TODO: the target triple must be changed to emit code for other
      // platforms
      std::string target_triple = llvm::sys::getProcessTriple();
      
      std::string error;
      const llvm::Target* target = llvm::TargetRegistry::lookupTarget(target_triple, error);

      if (target == nullptr)
      {
        FatalError(error.data(), __FILE__, __LINE__);
      }

      std::string cpu = "x86-64";
      std::string cpu_features = ""; // #TODO: find out the cpu features programmatically
      llvm::TargetOptions target_options; // #TODO: this works fine as default, but seems like a
                                          // useful structure for
                                          // optimization/profiling
      // #TODO: position independant code is a fine default,
      // however we should allow the user to change this via the command line
      llvm::Reloc::Model code_relocation_model = llvm::Reloc::Model::PIC_;
      // #TODO: the Small x86-64 code model is a fine default for x86-64,
      // however this must change if the code being compiled becomes larger 
      // than 2GB, or if the data within the code or being processed by the 
      // code becomes larger than 2GB. So we should add an option on the 
      // command line to select between models.
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

      std::shared_ptr<llvm::IRBuilder<>> builder = std::make_shared<llvm::IRBuilder<>>(*context);
      std::shared_ptr<llvm::Module> module = std::make_shared<llvm::Module>(options->input_file, *context);

      auto env = std::make_shared<Environment>(
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
          builder,
          target_machine,
          target_machine->createDataLayout()
          );

      InitializeBinopPrimitives(env);
      InitializeUnopPrimitives(env);

      return env;
    }
}
