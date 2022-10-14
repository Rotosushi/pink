#pragma once
#include "visitor/AstVisitor.h"

#include <string>

namespace pink {
class AstToString
    : public AstVisitorResult<AstToString, const Ast *, std::string>,
      public AstVisitor {
public:
  void Visit(Application *application) override;
  void Visit(Array *array) override;
  void Visit(Assignment *assignment) override;
  void Visit(Bind *bind) override;
  void Visit(Binop *binop) override;
  void Visit(Block *block) override;
  void Visit(Bool *boolean) override;
  void Visit(Conditional *conditional) override;
  void Visit(Dot *dot) override;
  void Visit(Function *function) override;
  void Visit(Int *integer) override;
  void Visit(Nil *nil) override;
  void Visit(Tuple *tuple) override;
  void Visit(Unop *unop) override;
  void Visit(Variable *variable) override;
  void Visit(While *loop) override;

  AstToString() = default;
  AstToString(const AstToString &other) = default;
  AstToString(AstToString &&other) = default;
  auto operator=(const AstToString &other) -> AstToString & = default;
  auto operator=(AstToString &&other) -> AstToString & = default;
  ~AstToString() override = default;
};
} // namespace pink