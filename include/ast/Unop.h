/**
 * @file Unop.h
 * @brief Header for class Unop
 * @version 0.1
 */
#pragma once
#include <string>

#include "ast/Ast.h"
#include "aux/StringInterner.h"

namespace pink {
/**
 * @brief Represents a unary operator expression
 *
 * \todo #CPP can be lowered to cpp unops
 */
class Unop : public Ast {
private:
  InternedString op;
  Ast::Pointer right;

public:
  Unop(const Location &location, InternedString opr,
       Ast::Pointer right) noexcept
      : Ast(Ast::Kind::Unop, location), op(opr), right(std::move(right)) {}
  ~Unop() noexcept override = default;
  Unop(const Unop &other) noexcept = delete;
  Unop(Unop &&other) noexcept = default;
  auto operator=(const Unop &other) noexcept -> Unop & = delete;
  auto operator=(Unop &&other) noexcept -> Unop & = default;

  auto GetOp() noexcept -> InternedString { return op; }
  auto GetOp() const noexcept -> InternedString { return op; }
  auto GetRight() noexcept -> Ast::Pointer & { return right; }
  auto GetRight() const noexcept -> const Ast::Pointer & { return right; }

  static auto classof(const Ast *ast) noexcept -> bool {
    return Ast::Kind::Unop == ast->GetKind();
  }

  void Accept(AstVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstAstVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
