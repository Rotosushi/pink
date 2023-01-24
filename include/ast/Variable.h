/**
 * @file Variable.h
 * @brief Header for class Variable
 * @version 0.1
 */
#pragma once
#include <string>

#include "ast/Ast.h"
#include "aux/StringInterner.h"

namespace pink {
/**
 * @brief Represents a Variable expression
 */
class Variable : public Ast {
private:
  InternedString symbol;

public:
  Variable(const Location &location, InternedString symbol) noexcept
      : Ast(Ast::Kind::Variable, location), symbol(symbol) {}
  ~Variable() noexcept override = default;
  Variable(const Variable &other) noexcept = delete;
  Variable(Variable &&other) noexcept = default;
  auto operator=(const Variable &other) noexcept -> Variable & = delete;
  auto operator=(Variable &&other) noexcept -> Variable & = default;

  auto GetSymbol() noexcept -> InternedString { return symbol; }
  auto GetSymbol() const noexcept -> InternedString { return symbol; }

  static auto classof(const Ast *ast) noexcept -> bool {
    return Ast::Kind::Variable == ast->GetKind();
  }

  void Accept(AstVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstAstVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
