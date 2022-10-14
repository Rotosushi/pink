#pragma once
#include "visitor/AstVisitor.h"

#include <string>

namespace pink {
class AstToString
    : public AstVisitorResult<AstToString, const Ast *, std::string>,
      public AstVisitor {
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
  void Visit(const Tuple *tuple) const override;
  void Visit(const Unop *unop) const override;
  void Visit(const Variable *variable) const override;
  void Visit(const While *loop) const override;

  AstToString() = default;
  AstToString(const AstToString &other) = default;
  AstToString(AstToString &&other) = default;
  auto operator=(const AstToString &other) -> AstToString & = default;
  auto operator=(AstToString &&other) -> AstToString & = default;
  ~AstToString() override = default;
};
} // namespace pink