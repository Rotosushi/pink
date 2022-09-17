#pragma once
#include <string>
#include <vector>

#include "llvm/IR/DataLayout.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/DIBuilder.h"

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
    
        #NOTE: #ASIDE: 9/13/2022
        We are using std::shared_ptr for the members of the environment over 
        std::unique_ptr, because we want to have the ability to build new 
        Environments which represent local scopes, and not reconstruct any 
        more data structures than we have to. for example, when we enter into 
        a function's scope, that functions symbol table will become the symbol
        table of the local environment, this environment can then be passed 
        into the Functions body->Codegen expression, such that the body is 
        generated with respect to the local environment of the function.
        Within this case we must construct the environment with a different 
        symbol table member than the scope of the function (the GlobalEnv),
        however, we don't need to reconstruct the parser, or the type internerer,
        or etc... these members 
        may simply be shared between the unique instances of the environment. Thus 
        std::shared_ptr fits the use case perfectly. and I (we?) am/are willing to 
        accept the overhead of the shared_ptr for each member. 

        the other benefiet would also extend were we to use std::unique_ptr, in that
        we want the environment to 'own' the members it holds. yet many members 
        of the environment require complex initialization routines which must be done
        on each construction, this is code want to place in a function, (NewGlobalEnv)
        and to be able to have such a function construct data and return it to an outer 
        scope it must be a return value, or transient argument, or heap allocated.
        since the environment is the 'hub' by which we access each of these members 
        we want the environment to 'own' the data that it holds. this begs the question,
        why not have them be simple members then? and the answer to that is the 
        large size of many of the members. it would be faster on construction and 
        take less memory to construct multiple environments composed of pointers,
        than multiple environments composed of direct members. (additionally, the 
        llvm::Context is a unique data structure per unit of compilation, that is
        we -never- want to duplicate this class. this is because it 'owns' many 
        of the core llvm data structures. such as the uniqued types and values.)
        so, since we want to use pointers, and we want ownsership, owning pointers 
        seemed the best solution.
    */

  /**
   * @brief [Environment] owns all of the data structures which are shared between the 
   *  different algorithms within the compiler.
   * 
   */
  class Environment {
  public:
    std::shared_ptr<std::vector<InternedString>> false_bindings;
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
    std::shared_ptr<llvm::IRBuilder<>> instruction_builder;
    //      std::shared_ptr<llvm::DIBuilder>   debug_builder;
    const llvm::Target*                target;
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
      std::shared_ptr<llvm::IRBuilder<>>           instruction_builder,
      //      std::shared_ptr<llvm::DIBuilder>             debug_builder,
      const llvm::Target*                          target,
      llvm::TargetMachine*                         target_machine,
      const llvm::DataLayout                       data_layout
    );
    // convience constructor for building an   
    // Environment around an inner scope. 
    Environment(
      const Environment& env,
      std::shared_ptr<SymbolTable> symbols
    );

    // convience constructor for building an   
    // Environment around an inner scope. 
    Environment(
      const Environment& env,
      std::shared_ptr<SymbolTable> symbols,
      std::shared_ptr<llvm::IRBuilder<>> builder,
      llvm::Function* current_function
    );
  };

  std::unique_ptr<Environment> NewGlobalEnv(std::shared_ptr<CLIOptions> options);
  std::unique_ptr<Environment> NewGlobalEnv(std::shared_ptr<CLIOptions> options, std::istream* instream);
}
