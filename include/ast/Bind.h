/**
 * @file Bind.h
 * @brief Header for class Bind
 * @version 0.1
 */
#pragma once

#include "ast/Ast.h"
#include "aux/StringInterner.h"

namespace pink {
/**
 * @brief Represents a Bind expression
 *
 */
class Bind : public Ast {
private:
  InternedString symbol;
  Ast::Pointer affix;

public:
  Bind(const Location &location, InternedString symbol,
       Ast::Pointer affix) noexcept
      : Ast(Ast::Kind::Bind, location), symbol(symbol),
        affix(std::move(affix)) {}
  ~Bind() noexcept override = default;
  Bind(const Bind &other) noexcept = delete;
  Bind(Bind &&other) noexcept = default;
  auto operator=(const Bind &other) noexcept -> Bind & = delete;
  auto operator=(Bind &&other) noexcept -> Bind & = default;

  auto GetSymbol() noexcept -> InternedString { return symbol; }
  auto GetSymbol() const noexcept -> InternedString { return symbol; }
  auto GetAffix() noexcept -> Ast::Pointer & { return affix; }
  auto GetAffix() const noexcept -> const Ast::Pointer & {
    return affix;
  }

  static auto classof(const Ast *ast) noexcept -> bool {
    return Ast::Kind::Bind == ast->GetKind();
  }

  void Accept(AstVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstAstVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
