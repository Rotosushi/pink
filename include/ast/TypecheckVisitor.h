#include "ast/Ast.h"
#include "ast/AstToString.h"

#include "aux/Environment.h"

#include "visitor/AstVisitor.h"
#include "visitor/VisitorResult.h"

#include "support/Find.h"
#include "support/LLVMValueToString.h"

#include <algorithm>

namespace pink {
class TypecheckVisitor
    : public ConstAstVisitor,
      public VisitorResult<const TypecheckVisitor, const Ast *,
                           Outcome<Type *, Error>> {
public:
  void Visit(const Application *application) const override;
  void Visit(const Array *array) const override;
  void Visit(const Assignment *assignment) const override;
  void Visit(const Bind *bind) const override;
  void Visit(const Binop *binop) const override;
  void Visit(const Block *block) const override;
  void Visit(const Bool *boolean) const override;
  void Visit(const Conditional *conditional) const override;
  void Visit(const Dot *dot) const override;
  void Visit(const Function *function) const override;
  void Visit(const Int *integer) const override;
  void Visit(const Nil *nil) const override;
  void Visit(const Subscript *subscript) const override;
  void Visit(const Tuple *tuple) const override;
  void Visit(const Unop *unop) const override;
  void Visit(const Variable *variable) const override;
  void Visit(const While *loop) const override;

  TypecheckVisitor(Environment &env) noexcept : env(env) {}
  TypecheckVisitor(const TypecheckVisitor &other) noexcept
      : VisitorResult(other), env(other.env) {}
  TypecheckVisitor(TypecheckVisitor &&other) noexcept
      : VisitorResult(std::move(other)), env(other.env) {}
  auto operator=(const TypecheckVisitor &other) noexcept -> TypecheckVisitor & {
    env = other.env;
    return *this;
  }

  auto operator=(TypecheckVisitor &&other) noexcept -> TypecheckVisitor & {
    env = other.env;
    return *this;
  }

  ~TypecheckVisitor() override = default;

private:
  Environment &env;
};
} // namespace pink