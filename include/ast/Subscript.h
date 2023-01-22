#pragma once
#include "ast/Ast.h"

namespace pink {
/**
 * @brief Implements array subscription
 */
class Subscript : public Ast {
private:
  [[nodiscard]] auto TypecheckV(const Environment &env) const
      -> Outcome<Type *, Error> override;

public:
  std::unique_ptr<Ast> left;
  std::unique_ptr<Ast> right;

  Subscript(const Location &location, std::unique_ptr<Ast> left,
            std::unique_ptr<Ast> right);

  ~Subscript() override = default;
  Subscript(const Subscript &other) = delete;
  Subscript(Subscript &&other) = default;
  auto operator=(const Subscript &other) -> Subscript & = delete;
  auto operator=(Subscript &&other) -> Subscript & = default;

  inline auto GetLeft() const -> const Ast * { return left.get(); }

  inline auto GetRight() const -> const Ast * { return right.get(); }

  /**
   * @brief This function is used to inplement llvm style [RTTI]
   * for this node kind
   *
   * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
   *
   * @param ast The Ast* being tested
   * @return true if ast *is* an instance of a [Subscript](#Subscript)
   * @return false if ast *is not* an instance of a [Subscript](#Subscript)
   */
  static auto classof(const Ast *ast) -> bool;

  /**
   * @brief returns the textual representation of a [Subscript](#Subscript)
   *
   * @return std::string left->ToString + "[" + right->ToString + "]"
   */
  [[nodiscard]] auto ToString() const -> std::string override;

  /**
   * @brief Performs the Array Subscript operation given the Array (or Slice)
   * on the left, and the Index on the right.
   *
   * @param env the Environment representing this compilation unit.
   * @return Outcome<llvm::Value *, Error> if true then the result of the
   * subscription operation, if false the Error encountered.
   */
  [[nodiscard]] auto Codegen(const Environment &env) const
      -> Outcome<llvm::Value *, Error> override;
};
} // namespace pink