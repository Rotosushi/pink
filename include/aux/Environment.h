/**
 * @file Environment.h
 * @brief Header for class Environment
 * @version 0.1
 *
 */
#pragma once
#include <string>
#include <vector>

#include "llvm/IR/DIBuilder.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#include "llvm/Target/TargetMachine.h"

#include "aux/CLIOptions.h"
#include "aux/Error.h"
#include "aux/Flags.h"
#include "aux/StringInterner.h"
#include "aux/SymbolTable.h"
#include "aux/TypeInterner.h"

#include "ops/BinopTable.h"
#include "ops/UnopTable.h"

#include "front/Parser.h"

namespace pink {

/**
 * @brief Environment owns all of the data structures which are shared between
 the different algorithms within the compiler.
 *
 *
 *
 * The Environment is the data structure that is passed
 * through each phase of the compiler, where it is used
 * to hold state for the future. So that the symbols,
 * bindings, types etc.. created are saved for later use.
 *
 * \note: 9/13/2022
 * We are using std::shared_ptr for the members of the environment over
 * std::unique_ptr, because we want to have the ability to build new
 * Environments which represent local scopes, and not reconstruct any
 * more data structures than we have to. for example, when we enter into
 * a function's scope, that functions symbol table will become the symbol
 * table of the local environment, this environment can then be passed
 * into the Functions body->Codegen expression, such that the body is
 * generated with respect to the local environment of the function.
 * Within this case we must construct the environment with a different
 * symbol table member than the scope of the function (the GlobalEnv),
 * however, we don't need to reconstruct the parser, or the type internerer,
 * or etc...
 * These members may simply be shared between the unique instances of the
 environment.
 * Thus std::shared_ptr fits the use case perfectly. and we are willing to
 * accept the overhead of the shared_ptr for each member.
 * the other benefit would also extend were we to use std::unique_ptr, in that
 * we want the environment to 'own' the members it holds. many members
 * of the environment require complex initialization routines which must be done
 * on each construction, this is code want to place in a function,
 [NewGlobalEnv](#NewGlobalEnv)
 * and to be able to have such a function construct data and return it to an
 outer
 * scope it must be a return value, or transient argument, or heap allocated.
 * since the environment is the 'hub' by which we access each of these members
 * we want the environment to 'own' the data that it holds. this begs the
 question,
 * why not have them be simple members then? and the answer to that is the
 * large size of many of the members. it would be faster on construction and
 * take less memory to construct multiple environments composed of pointers,
 * than multiple environments composed of direct members. (additionally, the
 * llvm::Context is a unique data structure per unit of compilation, that is
 * we -never- want to duplicate this class. this is because it 'owns' many
 * of the core llvm data structures. such as the uniqued types and values.)
 * so, since we want to use pointers, and we want ownsership, owning pointers
 * seemed the best solution.
 */
class Environment {
public:
  /**
   * @brief the [symbols](@ref InternedString) associated with
   * [bindings](#SymbolTable) constructed during [typechecking](@ref
   * Ast::Typecheck)
   *
   * we must keep track of these, because they are not complete bindings yet.
   * (hense the name 'false' bindings) in that they do not contain an associated
   * [value] within this symbol table. Which means that we must clean these
   * bindings up before we emit code, because the codegen routines for these
   * bindings will bind the symbols to their type *and* value, such that the
   * value may be used in later computation. If we were to not keep track of
   * these symbols, we would still need to solve the problem of how to
   * communicate the type of a declared symbol to later statements within a
   * scope, when the typechecking algorithm has no buisness constructing [value]
   * terms.
   *
   * [value]: https://llvm.org/doxygen/classllvm_1_1Value.html "llvm::Value*"
   */
  std::shared_ptr<std::vector<InternedString>> false_bindings;

  /**
   * @brief Internal flags concerning the current state of compilation.
   *
   * These flags are distinct from the flags controlling compilation held
   * within the [CLIOptions](#CLIOptions) in that these flags are not
   * controllable by the user on the command line, and instead are exclusively
   * set/reset during compilation of specific terms within a given [Ast](#Ast).
   */
  std::shared_ptr<TypecheckFlags> flags;

  /**
   * @brief The options associated with the current compilation task.
   *
   * In order to properly represent the options passed to the compiler by the
   * user this member must be constructed via a call to
   * [ParseCLIOptions](#ParseCLIOptions)
   */
  std::shared_ptr<CLIOptions> options;

  /**
   * @brief The Parser constructs [Ast](#Ast) terms from the associated input
   * stream.
   *
   * The [parse](#Parser::Parse) routine acts as an inverse operation for
   * [Ast::ToString](#Ast::ToString)
   */
  std::shared_ptr<Parser> parser;

  /**
   * @brief Interns each [symbol](#InternedString) read by the
   * [parser](#Parser).
   *
   * this structure ensures that we have one and only one copy of any
   * given symbol at any point within compilation. Thus we may use
   * pointer comparison instead of string comparison when performing
   * [lookup](#SymbolTable::Lookup).
   */
  std::shared_ptr<StringInterner> symbols;

  /**
   * @brief Interns each [operator](#InternedString) read by the
   * [parser](#Parser).
   *
   * this structure ensures that we have one and only one copy of any
   * given operator at any point within compilation. Thus we may use
   * pointer comparison instead of string comparison when performing
   * [lookup](#BinopTable::Lookup), or [lookup](#UnopTable::Lookup)
   *
   */
  std::shared_ptr<StringInterner> operators;

  /**
   * @brief Interns each [type](#Type) constructed during compilation.
   *
   * this structure ensures that we have one and only one copy of any
   * given [type](#Type) at any point within compilation. Thus we may
   * use pointer comparison instead of a deep tree comparison when performing
   * type equality during [typechecking](#Ast::Typecheck).
   *
   */
  std::shared_ptr<TypeInterner> types;

  /**
   * @brief Records the [type](#Type) and [value] of each
   * [symbol](#InternedString) which occurs within a [bind](#Bind) statement in
   * the source text.
   *
   * [value]: https://llvm.org/doxygen/classllvm_1_1Value.html "llvm::Value*"
   *
   * This is to record that binding, such that a [variable](#Variable) reference
   * can be [typed](#Ast::Typecheck) and associated with it's [value].
   *
   */
  std::shared_ptr<SymbolTable> bindings;

  /**
   * @brief Records all Binary Operators ([binop](#InternedString)s) known to
   * the compiler.
   *
   * This is to record that operator, such that the [parser](#Parser) may know
   * the [precedence](#Precedence) and [associativity](#Associativity) of the
   * operator, such that it can construct a [binop](#Binop) term from that
   * operator.
   *
   */
  std::shared_ptr<BinopTable> binops;

  /**
   * @brief Records all Unary Operators ([unop](#InternedString)s) known to the
   * compiler.
   *
   * This is to record that [operator](#InternedString), such that the
   * [parser](#Parser) may construct the [unop](#Unop) term from that
   * [operator](#InternedString)
   *
   */
  std::shared_ptr<UnopTable> unops;

  /**
   * @brief This is the [context] associated with this unit of compilation
   *
   * [context]: https://llvm.org/doxygen/classllvm_1_1LLVMContext.html
   * "llvm::Context"
   *
   * \warning The [context] *owns* many of the important and useful classes of
   * llvm itself, and as such a new [context] must only be constructed when a
   * new unit of compilation is similarly constructed. such as when threading
   * the compiler to translate many files simultaneously
   */
  std::shared_ptr<llvm::LLVMContext> context;

  /**
   * @brief This is the [llvm_module] associated with this unit of compilation
   *
   * [llvm_module]: https://llvm.org/doxygen/classllvm_1_1Module.html "Module"
   *
   * This class essentially represents a single file of source code.
   *
   */
  std::shared_ptr<llvm::Module> llvm_module;

  /**
   * @brief this is the [IRBuilder] which is used by [codegen] to construct the
   * [instruction]s corresponding to the [term] being compiled.
   *
   * [IRBuilder]: https://llvm.org/doxygen/classllvm_1_1IRBuilder.html
   * "llvm::IRBuilder" [codegen]: @ref Ast::Codegen "codegen" [instruction]:
   * https://llvm.org/doxygen/classllvm_1_1Instruction.html "llvm::Instruction"
   * [term]: @ref Ast "term"
   *
   */
  std::shared_ptr<llvm::IRBuilder<>> instruction_builder;

  // NOTE: 9/17/2022
  // we are close to adding debug information to the compiler,
  // but what stopped me is that we are planning on using a
  // custom calling convention in order to handle functions as values,
  // while any work we do with the debugger is assuming the C calling
  // convention. So we truly need to define our calling convention
  // before we could truly add debugging to the language.
  // however local variables would be no different from C,
  // so it is not as it debugging information would be completely
  // different from a C like languages debugger.
  // std::shared_ptr<llvm::DIBuilder>   debug_builder;

  /**
   * @brief This is the [Target] which represents some data relevant to the
   * target machine being compiled for.
   *
   * [Target]: https://llvm.org/doxygen/classllvm_1_1Target.html "Target"
   *
   * As far as we can tell, this class is constructed when our code calls
   * llvm::InitializeNativeTarget, and is what llvm calls a "managed static"
   * class. From the perspective of our code, this class is mainly used to
   * construct the [TargetMachine] for this unit of compilation.
   *
   * [TargetMachine]: https://llvm.org/doxygen/classllvm_1_1TargetMachine.html
   * "Target Machine"
   *
   */
  const llvm::Target *target;

  /**
   * @brief This is the [TargetMachine] which represents the data relevant to
   * constructing assembly for the target machine being compiled for.
   *
   * [TargetMachine]: https://llvm.org/doxygen/classllvm_1_1TargetMachine.html
   * "Target Machine"
   *
   */
  llvm::TargetMachine *target_machine;

  /**
   * @brief This is the [DataLayout] of the [TargetMachine] being compiled for.
   *
   * [DataLayout]: https://llvm.org/doxygen/classllvm_1_1DataLayout.html "Data
   * Layout" [TargetMachine]:
   * https://llvm.org/doxygen/classllvm_1_1TargetMachine.html "Target Machine"
   *
   * This class holds information such as the [Endianness] of the
   * [TargetMachine], the sizes of [llvmType]s on the [TargetMachine] and the
   * address spaces for pointers.
   *
   * [Endianness]: https://en.wikipedia.org/wiki/Endianness "Endianness"
   * [llvmTypes]: https://llvm.org/doxygen/classllvm_1_1Type.html "llvm::Type"
   *
   *
   */
  llvm::DataLayout data_layout;

  /**
   * @brief This holds the current [llvmFunction] being compiled
   *
   * [llvmFunction]: https://llvm.org/doxygen/classllvm_1_1Function.html
   * "llvm::Function"
   *
   * Intialized to nullptr, this member can be used to tell if we are
   * in global scope or a local scope, and as such it is used within
   * [various](#Bind::Codegen) [codegeneration](#While::Codegen) routines to
   * retrieve the [block]s they need to insert into.
   *
   * [block]: https://llvm.org/doxygen/classllvm_1_1BasicBlock.html "block"
   *
   * For instance this is set to the constructed [llvmFunction] when generating
   * the code for the body of a [function](#Function).
   *
   */
  llvm::Function *current_function;

private:
  /**
   * @brief Construct a new Environment
   *
   * it is safe to pass in default initialized values for the
   * [flags](#TypecheckFlags), [options](#CLIOptions), [parser](#Parser),
   * [symbols](#StringInterner), [operators](#StringInterner),
   * [types](#TypeInterner), [bindings](#SymbolTable), [binops](#BinopTable),
   * [unops](#UnopTable), and [llvmContext].
   *
   * [llvmContext]: https://llvm.org/doxygen/classllvm_1_1LLVMContext.html
   * "llvm::LLVMContext"
   *
   * The [Module] and [IRBuilder] must be constructed with respect to the
   * [llvmContext] being passed in.
   *
   * the [Target] must be from the [TargetRegistry], and the [TargetMachine]
   * must be constructed from that [Target].
   *
   * the [DataLayout] must be constructed from the [TargetMachine]
   *
   * [Module]: https://llvm.org/doxygen/classllvm_1_1Module.html "llvm::Module"
   * [IRBuilder]: https://llvm.org/doxygen/classllvm_1_1IRBuilder.html
   * "llvm::IRBuilder" [llvmContext]:
   * https://llvm.org/doxygen/classllvm_1_1LLVMContext.html "llvm::LLVMContext"
   * [Target]: https://llvm.org/doxygen/classllvm_1_1Target.html "llvm::Target"
   * [TargetRegistry]: https://llvm.org/doxygen/classllvm_1_1TargetRegistry.html
   * "llvm::TargetRegistry" [TargetMachine]:
   * https://llvm.org/doxygen/classllvm_1_1TargetMachine.html
   * "llvm:TargetMachine" [DataLayout]:
   * https://llvm.org/doxygen/classllvm_1_1DataLayout.html "llvm::DataLayout"
   *
   * @param flags
   * @param options
   * @param parser
   * @param symbols
   * @param operators
   * @param types
   * @param bindings
   * @param binops
   * @param unops
   * @param context
   * @param llvm_module
   * @param instruction_builder
   * @param target
   * @param target_machine
   * @param data_layout
   */
  Environment(std::shared_ptr<TypecheckFlags> flags,
              std::shared_ptr<CLIOptions> options,
              std::shared_ptr<Parser> parser,
              std::shared_ptr<StringInterner> symbols,
              std::shared_ptr<StringInterner> operators,
              std::shared_ptr<TypeInterner> types,
              std::shared_ptr<SymbolTable> bindings,
              std::shared_ptr<BinopTable> binops,
              std::shared_ptr<UnopTable> unops,
              std::shared_ptr<llvm::LLVMContext> context,
              std::shared_ptr<llvm::Module> llvm_module,
              std::shared_ptr<llvm::IRBuilder<>> instruction_builder,
              //      std::shared_ptr<llvm::DIBuilder> debug_builder,
              const llvm::Target *target, llvm::TargetMachine *target_machine,
              const llvm::DataLayout &data_layout);

  /**
   * @brief Construct a new Environment, as a copy of the given Environment
   *
   * This is a convenience constructor for creating a new local
   * Environment, say for the local scope of a [function](#Function).
   *
   * @param env The Environment to share members with
   * @param symbols The local scope of the new Environment.
   */
  Environment(const Environment &env, std::shared_ptr<SymbolTable> symbols);

  /**
   * @brief Construct a new Environment, as a copy of the given Environment
   *
   * This is a convenience constructor for creating a new local
   * Environment, say for the local scope of a [function](pink::Function).
   *
   *
   * @param env The Environment to share members with
   * @param symbols The local scope of the new Environment
   * @param builder The local IRBuilder of the new Environment
   * @param current_function The current [llvmFunction] which defines the
   * 'local' Environment.
   */
  Environment(const Environment &env, std::shared_ptr<SymbolTable> symbols,
              std::shared_ptr<llvm::IRBuilder<>> builder,
              llvm::Function *current_function);

  Environment(const Environment &env,
              std::shared_ptr<llvm::IRBuilder<>> builder);

public:
  /**
   * @brief Constructs a new global Environment. With all members initialized
   * for native codegeneration.
   *
   * @param options The command line [options](#CLIOptions).
   * @return std::unique_ptr<Environment> The new compilation environment.
   */
  static auto NewGlobalEnv(std::shared_ptr<CLIOptions> options)
      -> std::unique_ptr<Environment>;

  /**
   * @brief Constructs a new global Environment. With all members initialized
   * for native codegeneration.
   *
   * @param options The command line [options](#CLIOptions).
   * @param instream The input stream to [parse](#Parser) terms from.
   * @return std::unique_ptr<Environment> The new compilation environment.
   */
  static auto NewGlobalEnv(std::shared_ptr<CLIOptions> options,
                           std::istream *instream)
      -> std::unique_ptr<Environment>;

  
  static auto NewLocalEnv(const Environment &outer,
                          std::shared_ptr<SymbolTable> bindings)
      -> std::unique_ptr<Environment>;

  static auto NewLocalEnv(const Environment &outer,
                          std::shared_ptr<SymbolTable> bindings,
                          std::shared_ptr<llvm::IRBuilder<>> builder,
                          llvm::Function *function)
      -> std::unique_ptr<Environment>;

  static auto NewLocalEnv(const Environment &outer,
                          std::shared_ptr<llvm::IRBuilder<>> builder)
      -> std::unique_ptr<Environment>;
};

} // namespace pink
