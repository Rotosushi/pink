/**
 * @file While.h
 * @brief Header for class While
 * @version 0.1
 */
#pragma once

#include "ast/Ast.h"

namespace pink {
/**
 * @brief Represents an instance of a While loop
 */
class While : public Ast {
private:
  Ast::Pointer test;
  Ast::Pointer body;

public:
  While(const Location &location, Ast::Pointer test, Ast::Pointer body) noexcept
      : Ast(Ast::Kind::While, location), test(std::move(test)),
        body(std::move(body)) {}
  ~While() noexcept override = default;
  While(const While &other) noexcept = delete;
  While(While &&other) noexcept = default;
  auto operator=(const While &other) noexcept -> While & = delete;
  auto operator=(While &&other) noexcept -> While & = default;

  auto GetTest() noexcept -> Ast::Pointer & { return test; }
  auto GetTest() const noexcept -> const Ast::Pointer & { return test; }
  auto GetBody() noexcept -> Ast::Pointer & { return body; }
  auto GetBody() const noexcept -> const Ast::Pointer & { return body; }

  static auto classof(const Ast *ast) noexcept -> bool {
    return Ast::Kind::While == ast->GetKind();
  }

  void Accept(AstVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstAstVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
