#pragma once
#include "ast/Ast.h"

namespace pink {
/**
 * @brief Implements array subscription
 */
class Subscript : public Ast {
private:
  Ast::Pointer left;
  Ast::Pointer right;

public:
  Subscript(const Location &location, Ast::Pointer left,
            Ast::Pointer right) noexcept
      : Ast(Ast::Kind::Subscript, location), left(std::move(left)),
        right(std::move(right)) {}
  ~Subscript() noexcept override = default;
  Subscript(const Subscript &other) noexcept = delete;
  Subscript(Subscript &&other) noexcept = default;
  auto operator=(const Subscript &other) noexcept -> Subscript & = delete;
  auto operator=(Subscript &&other) noexcept -> Subscript & = default;

  auto GetLeft() noexcept -> Ast::Pointer & { return left; }
  auto GetLeft() const noexcept -> const Ast::Pointer & { return left; }
  auto GetRight() noexcept -> Ast::Pointer & { return right; }
  auto GetRight() const noexcept -> const Ast::Pointer & { return right; }

  static auto classof(const Ast *ast) noexcept -> bool {
    return Ast::Kind::Subscript == ast->GetKind();
  }

  void Accept(AstVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstAstVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink