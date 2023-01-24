/**
 * @file Assignment.h
 * @brief Header for class Assignment
 * @version 0.1
 */
#pragma once
#include "ast/Ast.h"

namespace pink {
/**
 * @brief Represents an Assignment expression
 */
class Assignment : public Ast {
private:
  Ast::Pointer left;
  Ast::Pointer right;

public:
  Assignment(const Location &location, Ast::Pointer left,
             Ast::Pointer right) noexcept
      : Ast(Ast::Kind::Assignment, location), left(std::move(left)),
        right(std::move(right)) {}
  ~Assignment() noexcept override = default;
  Assignment(const Assignment &other) noexcept = delete;
  Assignment(Assignment &&other) noexcept = default;
  auto operator=(const Assignment &other) noexcept -> Assignment & = delete;
  auto operator=(Assignment &&other) noexcept -> Assignment & = default;

  auto GetLeft() noexcept -> Ast::Pointer & { return left; }
  auto GetLeft() const noexcept -> const Ast::Pointer & { return left; }
  auto GetRight() noexcept -> Ast::Pointer & { return right; }
  auto GetRight() const noexcept -> const Ast::Pointer & { return right; }

  static auto classof(const Ast *ast) noexcept -> bool {
    return Ast::Kind::Assignment == ast->GetKind();
  }

  void Accept(AstVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstAstVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
