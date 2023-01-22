/**
 * @file Function.h
 * @brief Header for class Function
 * @version 0.1
 */
#pragma once
#include <utility>
#include <vector>

#include "ast/Ast.h"
#include "aux/StringInterner.h"
#include "aux/SymbolTable.h"

#include "type/FunctionType.h"

#include "llvm/IR/DerivedTypes.h"

namespace pink {
/**
 * @brief Represents an instance of a Function definition
 *
 *
 */
class Function : public Ast {
private:
  /**
   * @brief Compute the Type of this Function
   *
   * @param env the environment of this compilation unit
   * @return Outcome<Type*, Error> if true the Type of this Function,
   * if false the Error encountered.
   */
  [[nodiscard]] auto TypecheckV(const Environment &env) const
      -> Outcome<Type *, Error> override;

  /**
   * @brief Emit the instructions which return a value from the main function
   *
   * @param env  the environment of this compilation unit
   * @param body_value the value of the body, to be returned.
   * @return Outcome<llvm::Value *, Error> if true, nullptr, if false the error
   * encountered.
   */
  static void CodegenMainReturn(const Environment &env,
                                llvm::Value *body_value);

  /**
   * @brief Add the correct function parameter attributes given the defintion
   * of this function to the passed in llvm::Function.
   *
   * @param env the environment of this compilation unit
   * @param function the function to add parameter attributes too
   * @param function_type the llvm type of the function
   * @param p_function_type the pink type of the function
   * @return Outcome<llvm::Value *, Error> if true nullptr, if false the error
   * encountered
   */
  static void
  CodegenParameterAttributes(const Environment &env, llvm::Function *function,
                             const llvm::FunctionType *function_type,
                             const pink::FunctionType *p_function_type);

  /**
   * @brief Emit the instructions to create local variables for all arguments of
   * the given function
   *
   * @param env the environment of this compilation unit
   * @param function the function to emit arguments for
   * @param p_function_type the pink function type of the function
   */
  void CodegenArgumentInit(const Environment &env,
                           const llvm::Function *function,
                           const pink::FunctionType *p_function_type) const;

public:
  /**
   * @brief The name of this Function
   *
   */
  InternedString name;

  /**
   * @brief The formal arguments of this Function
   *
   */
  std::vector<std::pair<InternedString, Type *>> arguments;

  /**
   * @brief The Body expression of this Function
   *
   */
  std::unique_ptr<Ast> body;

  /**
   * @brief The local scope of this Function
   *
   */
  std::shared_ptr<SymbolTable> bindings;

  /**
   * @brief Construct a new Function
   *
   * @param location the textual location of this Function
   * @param name the name of this Function
   * @param arguments the formal arguments to this Function
   * @param body the Body expression of this Function
   * @param outer_scope the Scope this Function resides within
   */
  Function(const Location &location, const InternedString name,
           std::vector<std::pair<InternedString, Type *>> arguments,
           std::unique_ptr<Ast> body, SymbolTable *outer_scope);

  /**
   * @brief Destroy the Function
   *
   */
  ~Function() override = default;

  Function(const Function &other) = delete;

  Function(Function &&other) = default;

  auto operator=(const Function &other) -> Function & = delete;

  auto operator=(Function &&other) -> Function & = default;

  auto GetName() const -> InternedString { return name; }

  auto GetArguments() const
      -> const std::vector<std::pair<InternedString, Type *>> & {
    return arguments;
  }

  using iterator = std::vector<std::pair<InternedString, Type *>>::iterator;
  using const_iterator =
      std::vector<std::pair<InternedString, Type *>>::const_iterator;

  auto begin() -> iterator { return arguments.begin(); }
  auto cbegin() const -> const_iterator { return arguments.cbegin(); }
  auto end() -> iterator { return arguments.end(); }
  auto cend() const -> const_iterator { return arguments.cend(); }

  auto GetBody() const -> const Ast * { return body.get(); }

  auto GetSymbolTable() const -> const SymbolTable * { return bindings.get(); }

  /**
   * @brief Implements LLVM style [RTTI] for this class
   *
   * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
   *
   * @param ast the ast to test
   * @return true if ast *is* an instance of Function
   * @return false if ast *is not* an instance of Function
   */
  static auto classof(const Ast *ast) -> bool;

  /**
   * @brief Compute the cannonical string representation of this Function
   *
   * @return std::string the string representation
   */
  [[nodiscard]] auto ToString() const -> std::string override;

  /**
   * @brief Compute the value of this Function
   *
   * @param env the environment of this compilation unit
   * @return Outcome<llvm::Value*, Error> if true the llvm::Function
   * representing this Function, if false the Error encountered.
   */
  [[nodiscard]] auto Codegen(const Environment &env) const
      -> Outcome<llvm::Value *, Error> override;
};
} // namespace pink
