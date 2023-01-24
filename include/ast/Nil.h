/**
 * @file Nil.h
 * @brief Header for class Nil
 * @version 0.1
 *
 */
#pragma once
#include <iostream>
#include <string>

#include "ast/Ast.h"

namespace pink {
/**
 * @brief Represents the single valued type 'nil'
 *
 * it's value is equivalent to 'false.'
 * it's useful for many of the same reasons the
 * type 'void' is useful in C/C++ and has marginally
 * more uses than a true void type
 */
class Nil : public Ast {
public:
  Nil(const Location &location) noexcept : Ast(Ast::Kind::Nil, location) {}
  ~Nil() noexcept override = default;
  Nil(const Nil &other) noexcept = delete;
  Nil(Nil &&other) noexcept = default;
  auto operator=(const Nil &other) noexcept -> Nil & = delete;
  auto operator=(Nil &&other) noexcept -> Nil & = default;

  static auto classof(const Ast *ast) -> bool {
    return Ast::Kind::Nil == ast->GetKind();
  }

  void Accept(AstVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstAstVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
