/**
 * @file Binop.h
 * @brief Header for class Binop
 * @version 0.1
 */
#pragma once
#include <string>

#include "ast/Ast.h"

#include "ops/BinopTable.h"

#include "aux/StringInterner.h"

#include "type/ArrayType.h"
#include "type/IntType.h"
#include "type/SliceType.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"

namespace pink {
/**
 * @brief Represents single binary operator expression
 */
class Binop : public Ast {
private:
  InternedString op;
  Ast::Pointer left;
  Ast::Pointer right;

public:
  Binop(const Location &location, InternedString opr, Ast::Pointer left,
        Ast::Pointer right) noexcept
      : Ast(Ast::Kind::Binop, location), op(opr), left(std::move(left)),
        right(std::move(right)) {}
  ~Binop() noexcept override = default;
  Binop(const Binop &other) noexcept = delete;
  Binop(Binop &&other) noexcept = default;
  auto operator=(const Binop &other) noexcept -> Binop & = delete;
  auto operator=(Binop &&other) noexcept -> Binop & = default;

  auto GetOp() noexcept -> InternedString { return op; }
  auto GetOp() const noexcept -> InternedString { return op; }
  auto GetLeft() noexcept -> Ast::Pointer & { return left; }
  auto GetLeft() const noexcept -> const Ast::Pointer & { return left; }
  auto GetRight() noexcept -> Ast::Pointer & { return right; }
  auto GetRight() const noexcept -> const Ast::Pointer & { return right; }

  static auto classof(const Ast *ast) noexcept -> bool {
    return Ast::Kind::Binop == ast->GetKind();
  }

  void Accept(AstVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstAstVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
