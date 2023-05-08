// Copyright (C) 2023 cadence
//
// This file is part of pink.
//
// pink is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// pink is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with pink.  If not, see <http://www.gnu.org/licenses/>.

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
  Ast::Pointer   affix;

public:
  Bind(Location location, InternedString symbol, Ast::Pointer affix) noexcept
      : Ast(Ast::Kind::Bind, location),
        symbol(symbol),
        affix(std::move(affix)) {}
  ~Bind() noexcept override                            = default;
  Bind(const Bind &other) noexcept                     = delete;
  Bind(Bind &&other) noexcept                          = default;
  auto operator=(const Bind &other) noexcept -> Bind & = delete;
  auto operator=(Bind &&other) noexcept -> Bind      & = default;

  auto GetSymbol() noexcept -> InternedString { return symbol; }
  auto GetSymbol() const noexcept -> InternedString { return symbol; }
  auto GetAffix() noexcept -> Ast::Pointer & { return affix; }
  auto GetAffix() const noexcept -> const Ast::Pointer & { return affix; }

  static auto Create(Location       location,
                     InternedString symbol,
                     Ast::Pointer   affix) noexcept {
    return std::make_unique<Bind>(location, symbol, std::move(affix));
  }

  static auto classof(const Ast *ast) noexcept -> bool {
    return Ast::Kind::Bind == ast->GetKind();
  }

  auto Typecheck(CompilationUnit &unit) const noexcept
      -> Outcome<Type::Pointer> override;
  auto Codegen(CompilationUnit &unit) const noexcept
      -> Outcome<llvm::Value *> override;
  void Print(std::ostream &stream) const noexcept override;

  void Accept(AstVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstAstVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
