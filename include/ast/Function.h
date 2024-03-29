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
 * @file Function.h
 * @brief Header for class Function
 * @version 0.1
 */
#pragma once
#include <utility>
#include <vector>

#include "ast/Ast.h"
#include "aux/StringInterner.h"

#include "type/FunctionType.h"

namespace pink {
/**
 * @brief Represents an instance of a Function definition
 *
 *
 */
class Function : public Ast {
public:
  using Argument       = std::pair<InternedString, Type::Pointer>;
  using Arguments      = std::vector<Argument>;
  using iterator       = Arguments::iterator;
  using const_iterator = Arguments::const_iterator;

private:
  InternedString name;
  Arguments      arguments;
  Ast::Pointer   body;

public:
  Function(Location       location,
           InternedString name,
           Arguments      arguments,
           Ast::Pointer   body) noexcept
      : Ast(Ast::Kind::Function, location),
        name(name),
        arguments(std::move(arguments)),
        body(std::move(body)) {}
  ~Function() noexcept override                                = default;
  Function(const Function &other) noexcept                     = delete;
  Function(Function &&other) noexcept                          = default;
  auto operator=(const Function &other) noexcept -> Function & = delete;
  auto operator=(Function &&other) noexcept -> Function      & = default;

  static auto Create(Location       location,
                     InternedString name,
                     Arguments      arguments,
                     Ast::Pointer   body) noexcept {
    return std::make_unique<Function>(location,
                                      name,
                                      std::move(arguments),
                                      std::move(body));
  }

  auto GetName() noexcept -> InternedString { return name; }
  auto GetName() const noexcept -> InternedString { return name; }
  auto GetArguments() noexcept -> Arguments & { return arguments; }
  auto GetArguments() const noexcept -> const Arguments & { return arguments; }
  auto GetBody() noexcept -> Ast::Pointer & { return body; }
  auto GetBody() const noexcept -> const Ast::Pointer & { return body; }

  [[nodiscard]] auto begin() noexcept -> iterator { return arguments.begin(); }
  [[nodiscard]] auto begin() const noexcept -> const_iterator {
    return arguments.begin();
  }
  [[nodiscard]] auto cbegin() const noexcept -> const_iterator {
    return arguments.cbegin();
  }
  [[nodiscard]] auto end() noexcept -> iterator { return arguments.end(); }
  [[nodiscard]] auto end() const noexcept -> const_iterator {
    return arguments.end();
  }
  [[nodiscard]] auto cend() const noexcept -> const_iterator {
    return arguments.cend();
  }

  static auto classof(const Ast *ast) noexcept -> bool {
    return Ast::Kind::Function == ast->GetKind();
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
