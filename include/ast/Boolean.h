/**
 * @file Boolean.h
 * @brief Header for class Boolean
 * @version 0.1
 *
 */
#pragma once
#include <string>

#include "ast/Ast.h"

namespace pink {
/**
 * @brief [Boolean](#Boolean) is an [Ast](#Ast) node which represents a boolean
 * value
 *
 *
 * @headerfile Boolean.h "ast/Boolean.h"
 */
class Boolean : public Ast {
private:
  bool value;

public:
  Boolean(const Location &location, const bool value) noexcept
      : Ast(Ast::Kind::Boolean, location), value(value) {}
  ~Boolean() noexcept override = default;
  Boolean(const Boolean &other) noexcept = delete;
  Boolean(Boolean &&other) noexcept = default;
  auto operator=(const Boolean &other) noexcept -> Boolean & = delete;
  auto operator=(Boolean &&other) noexcept -> Boolean & = default;

  static auto classof(const Ast *ast) noexcept -> bool {
    return Ast::Kind::Boolean == ast->GetKind();
  }

  auto GetValue() const noexcept -> bool { return value; }

  void Accept(AstVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstAstVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
