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
 * @file Application.h
 * @brief Header for class Application
 * @version 0.1
 */
#pragma once
#include <vector> // std::vector

#include "ast/Ast.h"

namespace pink {
/**
 * @brief Represents an expression applying a function.
 *
 */
class Application : public Ast {
public:
  using Arguments      = std::vector<Ast::Pointer>;
  using iterator       = Arguments::iterator;
  using const_iterator = Arguments::const_iterator;

private:
  Ast::Pointer callee;
  Arguments    arguments;

public:
  Application(Location     location,
              Ast::Pointer callee,
              Arguments    arguments) noexcept
      : Ast(Ast::Kind::Application, location),
        callee(std::move(callee)),
        arguments(std::move(arguments)) {}
  ~Application() noexcept override                                   = default;
  Application(const Application &other) noexcept                     = delete;
  Application(Application &&other) noexcept                          = default;
  auto operator=(const Application &other) noexcept -> Application & = delete;
  auto operator=(Application &&other) noexcept -> Application      & = default;

  static auto
  Create(Location location, Ast::Pointer callee, Arguments arguments) noexcept {
    return std::make_unique<Application>(location,
                                         std::move(callee),
                                         std::move(arguments));
  }

  [[nodiscard]] auto GetCallee() noexcept -> Ast::Pointer & { return callee; }
  [[nodiscard]] auto GetCallee() const noexcept -> const Ast::Pointer & {
    return callee;
  }
  [[nodiscard]] auto GetArguments() noexcept -> Arguments & {
    return arguments;
  }
  [[nodiscard]] auto GetArguments() const noexcept -> const Arguments & {
    return arguments;
  }

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

  static auto classof(const Ast *ast) -> bool {
    return ast->GetKind() == Ast::Kind::Application;
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
