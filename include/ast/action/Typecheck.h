#pragma once

#include "ast/Ast.h"
#include "type/Type.h"

#include "aux/Error.h"
#include "aux/Outcome.h"

#include "ast/visitor/AstVisitor.h"
#include "visitor/VisitorResult.h"

namespace pink {
using TypecheckResult = Outcome<Type::Pointer, Error>;
class TypecheckVisitor
    : public ConstVisitorResult<TypecheckVisitor, const Ast::Pointer &,
                                TypecheckResult>,
      public ConstAstVisitor {
private:
  Environment &env;

public:
  void Visit(const Application *application) const noexcept override;
  void Visit(const Array *array) const noexcept override;
  void Visit(const Assignment *assignment) const noexcept override;
  void Visit(const Bind *bind) const noexcept override;
  void Visit(const Binop *binop) const noexcept override;
  void Visit(const Block *block) const noexcept override;
  void Visit(const Boolean *boolean) const noexcept override;
  void Visit(const Conditional *conditional) const noexcept override;
  void Visit(const Dot *dot) const noexcept override;
  void Visit(const Function *function) const noexcept override;
  void Visit(const Integer *integer) const noexcept override;
  void Visit(const Nil *nil) const noexcept override;
  void Visit(const Subscript *subscript) const noexcept override;
  void Visit(const Tuple *tuple) const noexcept override;
  void Visit(const Unop *unop) const noexcept override;
  void Visit(const Variable *variable) const noexcept override;
  void Visit(const While *loop) const noexcept override;

  TypecheckVisitor(Environment &env) noexcept : env(env) {}
  ~TypecheckVisitor() noexcept override = default;
  TypecheckVisitor(const TypecheckVisitor &other) noexcept = default;
  TypecheckVisitor(TypecheckVisitor &&other) noexcept = default;
  auto operator=(const TypecheckVisitor &other) noexcept
      -> TypecheckVisitor & = delete;
  auto operator=(TypecheckVisitor &&other) noexcept
      -> TypecheckVisitor & = delete;
};

/**
 * @brief Computes the [Type](#Type) of this [term](#Ast)
 *
 * This function runs the, as far as I know, standard simple typing algorithm,
 * eagerly evaluated, with no implicit casting. There are currently
 * no syntactic forms which allow for user types. (other than functions)
 *
 * \todo implement some form of user defined types
 *
 * @param ast the Ast to Typecheck
 * @param env The Environment to typecheck against, an
 * [Environment](#Environment) set up as if by [NewGlobalEnv](#NewGlobalEnv)
 * is suitable
 * @return Outcome<Type*, Error> if the type checking algorithm could assign
 * this term a [type](#Type) then the Outcome holds that type, otherwise the
 * Outcome holds the Error that the type checking algorithm encountered.
 */
[[nodiscard]] auto Typecheck(const Ast::Pointer &ast, Environment &env) noexcept
    -> TypecheckResult;
} // namespace pink
