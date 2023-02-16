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
private:
  std::size_t      gensym_counter;
  EnvironmentFlags internal_flags;
  CLIOptions       cli_options;
  Parser           parser;
  StringInterner   variable_interner;
  StringInterner   operator_interner;
  TypeInterner     type_interner;
  ScopeStack       scopes;
  BinopTable       binop_table;
  UnopTable        unop_table;

  // exposing llvm member interfaces (they are quite large)
public:
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

public:
  ~Environment()                                            = default;
  Environment(const Environment &other)                     = delete;
  Environment(Environment &&other)                          = default;
  auto operator=(const Environment &other) -> Environment & = delete;
  auto operator=(Environment &&other) -> Environment      & = default;

  // Environment methods
  auto Gensym(std::string_view prefix = "__") -> InternedString;

  void PrintErrorWithSourceText(std::ostream &out, const Error &error) const {
    auto bad_source = parser.ExtractSourceLine(error.location);
    error.Print(out, bad_source);
  }

  auto EmitFiles(std::ostream &err) const -> int;
  auto EmitObjectFile(std::ostream &err) const -> int;
  auto EmitAssemblyFile(std::ostream &err) const -> int;

  static auto NativeCPUFeatures() noexcept -> std::string;
  static auto CreateNativeEnvironment(CLIOptions cli_options) -> Environment;
  static auto CreateNativeEnvironment() -> Environment;

  void DeclareVariable(std::string_view symbol,
                       Type::Pointer    type,
                       llvm::Value     *value = nullptr) {
    const auto *name = variable_interner.Intern(symbol);
    BindVariable(name, type, value);
  }

  // exposing EnvironmentFlags interface
  [[nodiscard]] auto OnTheLHSOfAssignment() const -> bool {
    return internal_flags.OnTheLHSOfAssignment();
  }
  auto OnTheLHSOfAssignment(bool state) -> bool {
    return internal_flags.OnTheLHSOfAssignment(state);
  }

  [[nodiscard]] auto WithinAddressOf() const -> bool {
    return internal_flags.WithinAddressOf();
  }
  auto WithinAddressOf(bool state) -> bool {
    return internal_flags.WithinAddressOf(state);
  }

  [[nodiscard]] auto WithinDereferencePtr() const -> bool {
    return internal_flags.WithinDereferencePtr();
  }
  auto WithinDereferencePtr(bool state) -> bool {
    return internal_flags.WithinDereferencePtr(state);
  }

  [[nodiscard]] auto WithinBindExpression() const -> bool {
    return internal_flags.WithinBindExpression();
  }
  auto WithinBindExpression(bool state) -> bool {
    return internal_flags.WithinBindExpression(state);
  }

  // exposing CLIOptions interface
  [[nodiscard]] auto DoVerbose() const noexcept -> bool {
    return cli_options.DoVerbose();
  }
  [[nodiscard]] auto DoLink() const noexcept -> bool {
    return cli_options.DoLink();
  }
  [[nodiscard]] auto DoEmitObject() const noexcept -> bool {
    return cli_options.DoEmitObject();
  }
  [[nodiscard]] auto DoEmitAssembly() const noexcept -> bool {
    return cli_options.DoEmitAssembly();
  }

  [[nodiscard]] auto GetInputFilename() const -> std::string_view {
    return cli_options.GetInputFilename();
  }
  [[nodiscard]] auto GetExecutableFilename() const -> std::string_view {
    return cli_options.GetExecutableFilename();
  }
  [[nodiscard]] auto GetObjectFilename() const -> std::string_view {
    return cli_options.GetObjectFilename();
  }
  [[nodiscard]] auto GetAssemblyFilename() const -> std::string_view {
    return cli_options.GetAssemblyFilename();
  }

  [[nodiscard]] auto GetOptimizationLevel() const -> llvm::OptimizationLevel {
    return cli_options.GetOptimizationLevel();
  }

  // exposing Parser's interface
  [[nodiscard]] auto EndOfInput() const -> bool { return parser.EndOfInput(); }
  [[nodiscard]] auto ExtractSourceLine(const Location &location) const
      -> std::string_view {
    return parser.ExtractSourceLine(location);
  }
  [[nodiscard]] auto GetIStream() const -> std::istream * {
    return parser.GetIStream();
  }
  void SetIStream(std::istream *input_stream) {
    return parser.SetIStream(input_stream);
  }
  auto Parse() -> Parser::Result { return parser.Parse(*this); }

  // exposing variable_interner's interface
  auto InternVariable(std::string_view str) -> InternedString {
    return variable_interner.Intern(str);
  }

  // exposing operator_interner's interface
  auto InternOperator(std::string_view str) -> InternedString {
    return operator_interner.Intern(str);
  }

  // exposing TypeInterner's interface
  auto GetNilType() -> NilType::Pointer { return type_interner.GetNilType(); }
  auto GetBoolType() -> BooleanType::Pointer {
    return type_interner.GetBoolType();
  }
  auto GetIntType() -> IntegerType::Pointer {
    return type_interner.GetIntType();
  }
  auto GetCharacterType() -> CharacterType::Pointer {
    return type_interner.GetCharacterType();
  }
  auto GetVoidType() -> VoidType::Pointer {
    return type_interner.GetVoidType();
  }

  auto GetFunctionType(Type::Pointer                  ret_type,
                       FunctionType::Arguments const &arg_types)
      -> FunctionType::Pointer {
    return type_interner.GetFunctionType(ret_type, arg_types);
  }
  auto GetFunctionType(Type::Pointer             ret_type,
                       FunctionType::Arguments &&arg_types)
      -> FunctionType::Pointer {
    return type_interner.GetFunctionType(ret_type, std::move(arg_types));
  }

  auto GetPointerType(Type::Pointer pointee_type) -> PointerType::Pointer {
    return type_interner.GetPointerType(pointee_type);
  }
  auto GetSliceType(Type::Pointer pointee_type) -> SliceType::Pointer {
    return type_interner.GetSliceType(pointee_type);
  }

  auto GetArrayType(std::size_t size, Type::Pointer element_type)
      -> ArrayType::Pointer {
    return type_interner.GetArrayType(size, element_type);
  }

  auto GetTupleType(TupleType::Elements const &elements) -> TupleType::Pointer {
    return type_interner.GetTupleType(elements);
  }
  auto GetTupleType(TupleType::Elements &&elements) -> TupleType::Pointer {
    return type_interner.GetTupleType(std::move(elements));
  }

  auto GetTextType(std::size_t length) -> ArrayType::Pointer {
    return type_interner.GetTextType(length);
  }

  // exposing ScopeStack's interface
  void PushScope() { scopes.PushScope(); }
  void PopScope() { scopes.PopScope(); }

  auto LookupVariable(InternedString symbol) const
      -> std::optional<SymbolTable::Value> {
    return scopes.Lookup(symbol);
  }
  auto LookupLocalVariable(InternedString symbol) const
      -> std::optional<SymbolTable::Value> {
    return scopes.LookupLocal(symbol);
  }
  void
  BindVariable(InternedString symbol, Type::Pointer type, llvm::Value *value) {
    scopes.Bind(symbol, type, value);
  }

  // exposing BinopTable's interface
  auto RegisterBinop(InternedString opr,
                     Precedence     precedence,
                     Associativity  associativity) -> BinopLiteral * {
    return binop_table.Register(opr, precedence, associativity);
  }

  auto RegisterBinop(InternedString opr,
                     Precedence     precedence,
                     Associativity  associativity,
                     Type::Pointer  left_t,
                     Type::Pointer  right_t,
                     Type::Pointer  ret_t,
                     BinopCodegenFn fn_p) -> BinopLiteral * {
    return binop_table
        .Register(opr, precedence, associativity, left_t, right_t, ret_t, fn_p);
  }

  auto LookupBinop(InternedString opr) -> std::optional<BinopLiteral *> {
    return binop_table.Lookup(opr);
  }

  // exposing UnopTable's interface
  auto RegisterUnop(InternedString opr) -> UnopLiteral * {
    return unop_table.Register(opr);
  }
  auto RegisterUnop(InternedString opr,
                    Type::Pointer  arg_t,
                    Type::Pointer  ret_t,
                    UnopCodegenFn  fn_p) -> UnopLiteral * {
    return unop_table.Register(opr, arg_t, ret_t, fn_p);
  }

  auto LookupUnop(InternedString opr) -> std::optional<UnopLiteral *> {
    return unop_table.Lookup(opr);
  }
};

} // namespace pink
