/**
 * @file IfThenElse.h
 * @brief Header for class IfThenElse
 * @version 0.1
 */
#pragma once

#include "ast/Ast.h"

namespace pink {
/**
 * @brief Represents an instance of a conditional expression
 */
class IfThenElse : public Ast {
private:
  Ast::Pointer test;
  Ast::Pointer first;
  Ast::Pointer second;

public:
  IfThenElse(const Location &location,
             Ast::Pointer    test,
             Ast::Pointer    first,
             Ast::Pointer    second) noexcept
      : Ast(Ast::Kind::IfThenElse, location),
        test(std::move(test)),
        first(std::move(first)),
        second(std::move(second)) {}
  ~IfThenElse() noexcept override                                  = default;
  IfThenElse(const IfThenElse &other) noexcept                     = delete;
  IfThenElse(IfThenElse &&other) noexcept                          = default;
  auto operator=(const IfThenElse &other) noexcept -> IfThenElse & = delete;
  auto operator=(IfThenElse &&other) noexcept -> IfThenElse      & = default;

  auto GetTest() noexcept -> Ast::Pointer & { return test; }
  auto GetTest() const noexcept -> const Ast::Pointer & { return test; }
  auto GetFirst() noexcept -> Ast::Pointer & { return first; }
  auto GetFirst() const noexcept -> const Ast::Pointer & { return first; }
  auto GetSecond() noexcept -> Ast::Pointer & { return second; }
  auto GetSecond() const noexcept -> const Ast::Pointer & { return second; }

  static auto classof(const Ast *ast) noexcept -> bool {
    return Ast::Kind::IfThenElse == ast->GetKind();
  }

  void Accept(AstVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstAstVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
