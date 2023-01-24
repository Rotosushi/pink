/**
 * @file Block.h
 * @brief Header for class Block
 * @version 0.1
 */
#pragma once
#include <vector>

#include "aux/SymbolTable.h"

#include "ast/Ast.h"

namespace pink {
/**
 * @brief Represents a new scope of expressions
 */
class Block : public Ast {
public:
  using ExpressionsType = std::vector<Ast::Pointer>;
  using iterator = ExpressionsType::iterator;
  using const_iterator = ExpressionsType::const_iterator;

private:
  ExpressionsType expressions;
  SymbolTable::Pointer scope;

public:
  Block(const Location &location, SymbolTable *outer_scope) noexcept
      : Ast(Ast::Kind::Block, location), scope(outer_scope) {}
  Block(const Location &location, ExpressionsType expressions,
        SymbolTable *outer_scope) noexcept
      : Ast(Ast::Kind::Block, location), expressions(std::move(expressions)),
        scope(outer_scope) {}
  ~Block() noexcept override = default;
  Block(const Block &other) noexcept = delete;
  Block(Block &&other) noexcept = default;
  auto operator=(const Block &other) noexcept -> Block & = delete;
  auto operator=(Block &&other) noexcept -> Block & = default;

  auto GetExpressions() noexcept -> ExpressionsType & { return expressions; }
  auto GetExpressions() const noexcept -> const ExpressionsType & {
    return expressions;
  }
  auto GetScope() noexcept -> SymbolTable::Pointer & { return scope; }
  auto GetScope() const noexcept -> const SymbolTable::Pointer & {
    return scope;
  }

  [[nodiscard]] auto begin() noexcept -> iterator {
    return expressions.begin();
  }
  [[nodiscard]] auto begin() const noexcept -> const_iterator {
    return expressions.begin();
  }
  [[nodiscard]] auto cbegin() const noexcept -> const_iterator {
    return expressions.cbegin();
  }
  [[nodiscard]] auto end() noexcept -> iterator { return expressions.end(); }
  [[nodiscard]] auto end() const noexcept -> const_iterator {
    return expressions.end();
  }
  [[nodiscard]] auto cend() const noexcept -> const_iterator {
    return expressions.cend();
  }

  static auto classof(const Ast *ast) noexcept -> bool {
    return Ast::Kind::Block == ast->GetKind();
  }

  void Accept(AstVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstAstVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
