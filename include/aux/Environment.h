/**
 * @file Environment.h
 * @brief Header for class Environment
 * @version 0.1
 *
 */
#pragma once
#include <list>
#include <optional>
#include <stack>
#include <string>
#include <vector>

// #include "llvm/IR/DIBuilder.h"
// #include "llvm/IR/DataLayout.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Target/TargetMachine.h"

#include "aux/CLIOptions.h"
#include "aux/Error.h"
#include "aux/InternalFlags.h"
#include "aux/ScopeStack.h"
#include "aux/StringInterner.h"
#include "aux/TypeInterner.h"

#include "ops/BinopTable.h"
#include "ops/UnopTable.h"

#include "front/Parser.h"

namespace pink {

/**
 * @brief Environment owns all of the data structures which are shared between
 * the different algorithms within the compiler. Essentially the Environment
 * exists to keep the call signatures of these algorithms smaller, as the other
 * option is to pass the needed data structures in by reference anyways.
 * another benefiet is that it provides a convienent place for placing the
 * initialization work of the llvm data structures.
 */
class Environment {
public:
  std::size_t                        gensym_counter;
  InternalFlags                      internal_flags;
  CLIOptions                         cli_options;
  Parser                             parser;
  StringInterner                     variable_interner;
  StringInterner                     operator_interner;
  TypeInterner                       type_interner;
  ScopeStack                         scopes;
  BinopTable                         binop_table;
  UnopTable                          unop_table;
  std::unique_ptr<llvm::LLVMContext> context;
  std::unique_ptr<llvm::Module>      module;
  std::unique_ptr<llvm::IRBuilder<>> instruction_builder;
  llvm::TargetMachine               *target_machine;
  llvm::Function                    *current_function;
  // 2/6/2023
  // we still are not ready to add debug information just
  // yet. even though we cannot implement functions as values
  // as we originally hoped.
  // std::shared_ptr<llvm::DIBuilder>   debug_builder;

private:
  Environment(CLIOptions                         cli_options,
              std::unique_ptr<llvm::LLVMContext> context,
              std::unique_ptr<llvm::Module>      module,
              std::unique_ptr<llvm::IRBuilder<>> instruction_builder,
              llvm::TargetMachine               *target_machine)
      : cli_options(std::move(cli_options)),
        context(std::move(context)),
        module(std::move(module)),
        instruction_builder(std::move(instruction_builder)),
        target_machine(target_machine) {
    assert(target_machine != nullptr);
  }

  static auto NativeCPUFeatures() noexcept -> std::string;

public:
  ~Environment()                                            = default;
  Environment(const Environment &other)                     = delete;
  Environment(Environment &&other)                          = default;
  auto operator=(const Environment &other) -> Environment & = delete;
  auto operator=(Environment &&other) -> Environment      & = default;

  // #NOTE 2/7/2023
  // The current litmnus test for if a routine needs to be a
  // member of the environment or can exist as a free function
  // is fuzzy. Gensym makes sense, as it makes the counter
  // per module, so there is no threading concern if we want
  // to compile environments on different threads.
  //
  // generate a new symbol, valid for the current scope
  auto Gensym(std::string_view prefix = "__") -> InternedString;

  void PrintErrorWithSourceText(std::ostream &out, const Error &error) const {
    auto bad_source = parser.ExtractLine(error.location);
    error.Print(out, bad_source);
  }

  auto EmitFiles(std::ostream &err) const -> int;
  auto EmitObjectFile(std::ostream &err) const -> int;
  auto EmitAssemblyFile(std::ostream &err) const -> int;

  /**
   * @brief Constructs a new global Environment. With all members initialized
   * for native codegeneration.
   *
   * @param cli_options The command line [cli_options](#CLIOptions).
   * @return Environment The new compilation environment.
   */
  static auto CreateNativeGlobalEnv(CLIOptions cli_options) -> Environment;

  static auto CreateNativeGlobalEnv() -> Environment;
};

} // namespace pink
