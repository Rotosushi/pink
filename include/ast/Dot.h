/**
 * @file Dot.h
 * @brief Header for class Dot
 * @version 0.1
 */
#pragma once
#include "ast/Ast.h"

namespace pink {
/**
 * @brief Represents a member access expression.
 */
class Dot : public Ast {
private:
  Ast::Pointer left;
  Ast::Pointer right;

public:
  Dot(const Location &location, Ast::Pointer left, Ast::Pointer right) noexcept
      : Ast(Ast::Kind::Dot, location), left(std::move(left)),
        right(std::move(right)) {}
  ~Dot() noexcept override = default;
  Dot(const Dot &other) noexcept = delete;
  Dot(Dot &&other) noexcept = default;
  auto operator=(const Dot &other) noexcept -> Dot & = delete;
  auto operator=(Dot &&other) noexcept -> Dot & = default;

  auto GetLeft() noexcept -> Ast::Pointer & { return left; }
  auto GetLeft() const noexcept -> const Ast::Pointer & { return left; }
  auto GetRight() noexcept -> Ast::Pointer & { return right; }
  auto GetRight() const noexcept -> const Ast::Pointer & { return right; }

  static auto classof(const Ast *ast) noexcept -> bool {
    return Ast::Kind::Dot == ast->GetKind();
  }

  void Accept(AstVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstAstVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
