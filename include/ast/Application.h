/**
 * @file Application.h
 * @brief Header for class Application
 * @version 0.1
 */
#pragma once
#include <memory> // std::unique_ptr
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
  Application(const Location &location,
              Ast::Pointer    callee,
              Arguments       arguments) noexcept
      : Ast(Ast::Kind::Application, location),
        callee(std::move(callee)),
        arguments(std::move(arguments)) {}
  ~Application() noexcept override                                   = default;
  Application(const Application &other) noexcept                     = delete;
  Application(Application &&other) noexcept                          = default;
  auto operator=(const Application &other) noexcept -> Application & = delete;
  auto operator=(Application &&other) noexcept -> Application      & = default;

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

  void Accept(AstVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstAstVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
