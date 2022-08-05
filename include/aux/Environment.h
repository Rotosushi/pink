#pragma once
#include <string>
#include <vector>

#include "llvm/IR/DataLayout.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

#include "llvm/Target/TargetMachine.h"

#include "aux/Error.h"
#include "aux/StringInterner.h"
#include "aux/SymbolTable.h"
#include "aux/TypeInterner.h"
#include "aux/CLIOptions.h"
#include "aux/Flags.h"

#include "ops/BinopTable.h"
#include "ops/UnopTable.h"

#include "front/Parser.h"



namespace pink {
    /*
        The Environment is the data structure that is passed
        through each phase of the compiler, where it is used
        to hold state for the future. So that the symbols,
        bindings, types etc.. created are saved for later use.
    */
    class Environment {
    public:
      std::vector<InternedString>        false_bindings;
      std::shared_ptr<Flags>             flags;
      std::shared_ptr<CLIOptions>        options;
      std::shared_ptr<Parser>            parser;
      std::shared_ptr<StringInterner>    symbols;
      std::shared_ptr<StringInterner>    operators;
      std::shared_ptr<TypeInterner>      types;
      std::shared_ptr<SymbolTable>       bindings;
      std::shared_ptr<BinopTable>        binops;
      std::shared_ptr<UnopTable>         unops;
      std::shared_ptr<llvm::LLVMContext> context;
      std::shared_ptr<llvm::Module>      module;
      std::shared_ptr<llvm::IRBuilder<>> builder;
      llvm::TargetMachine*               target_machine;
      const llvm::DataLayout             data_layout;
      llvm::Function*                    current_function;

      Environment(
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
        );
      // convience constructor for building an   
      // Environment around an inner scope. 
      Environment(
        std::shared_ptr<Environment> env,
        std::shared_ptr<SymbolTable> symbols
        );
      
      // convience constructor for building an   
      // Environment around an inner scope. 
      Environment(
        std::shared_ptr<Environment> env,
        std::shared_ptr<SymbolTable> symbols,
        std::shared_ptr<llvm::IRBuilder<>> builder,
        llvm::Function* current_function
        );
    };

    std::shared_ptr<Environment> NewGlobalEnv(std::shared_ptr<CLIOptions> options);
    std::shared_ptr<Environment> NewGlobalEnv(std::shared_ptr<CLIOptions> options, std::istream* instream);

}
