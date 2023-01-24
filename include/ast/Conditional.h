/**
 * @file Conditional.h
 * @brief Header for class Conditional
 * @version 0.1
 */
#pragma once

#include "ast/Ast.h"

namespace pink {
/**
 * @brief Represents an instance of a conditional expression
 */
class Conditional : public Ast {
private:
  Ast::Pointer test;
  Ast::Pointer first;
  Ast::Pointer second;

public:
  Conditional(const Location &location, Ast::Pointer test, Ast::Pointer first,
              Ast::Pointer second) noexcept
      : Ast(Ast::Kind::Conditional, location), test(std::move(test)),
        first(std::move(first)), second(std::move(second)) {}
  ~Conditional() noexcept override = default;
  Conditional(const Conditional &other) noexcept = delete;
  Conditional(Conditional &&other) noexcept = default;
  auto operator=(const Conditional &other) noexcept -> Conditional & = delete;
  auto operator=(Conditional &&other) noexcept -> Conditional & = default;

  auto GetTest() noexcept -> Ast::Pointer & { return test; }
  auto GetTest() const noexcept -> const Ast::Pointer & { return test; }
  auto GetFirst() noexcept -> Ast::Pointer & { return first; }
  auto GetFirst() const noexcept -> const Ast::Pointer & { return first; }
  auto GetSecond() noexcept -> Ast::Pointer & { return second; }
  auto GetSecond() const noexcept -> const Ast::Pointer & { return second; }

  static auto classof(const Ast *ast) noexcept -> bool {
    return Ast::Kind::Conditional == ast->GetKind();
  }

  void Accept(AstVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstAstVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
