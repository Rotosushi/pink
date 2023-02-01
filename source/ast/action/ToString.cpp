

#include "ast/action/ToString.h"
#include "type/action/ToString.h"

#include "ast/All.h"

namespace pink {
class AstToStringVisitor
    : public ConstVisitorResult<AstToStringVisitor, const Ast::Pointer &,
                                std::string>,
      public ConstAstVisitor {
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

  AstToStringVisitor() noexcept                                = default;
  ~AstToStringVisitor() override                               = default;
  AstToStringVisitor(const AstToStringVisitor &other) noexcept = default;
  AstToStringVisitor(AstToStringVisitor &&other) noexcept      = default;
  auto operator=(const AstToStringVisitor &other) noexcept
      -> AstToStringVisitor & = default;
  auto operator=(AstToStringVisitor &&other) noexcept
      -> AstToStringVisitor & = default;
};

void AstToStringVisitor::Visit(const Application *application) const noexcept {
  result = Compute(application->GetCallee(), this);
  result += "(";

  std::size_t index  = 0;
  std::size_t length = application->GetArguments().size();
  for (const auto &argument : application->GetArguments()) {
    result += Compute(argument, this);

    if (index < (length - 1)) {
      result += ", ";
    }
    index++;
  }

  result += ")";
}

void AstToStringVisitor::Visit(const Array *array) const noexcept {
  result = "[";

  std::size_t index  = 0;
  std::size_t length = array->GetElements().size();
  for (const auto &element : array->GetElements()) {
    result += Compute(element, this);

    if (index < (length - 1)) {
      result += ", ";
    }
    index++;
  }

  result += "]";
}

void AstToStringVisitor::Visit(const Assignment *assignment) const noexcept {
  result = Compute(assignment->GetLeft(), this);
  result += " = ";
  result += Compute(assignment->GetRight(), this);
}

void AstToStringVisitor::Visit(const Bind *bind) const noexcept {
  result = bind->GetSymbol();
  result += " := ";
  result += Compute(bind->GetAffix(), this);
}

void AstToStringVisitor::Visit(const Binop *binop) const noexcept {
  if (llvm::isa<Binop>(binop->GetLeft())) {
    result += "(";
    result += Compute(binop->GetLeft(), this);
    result += ")";
  } else {
    result += Compute(binop->GetLeft(), this);
  }

  result += " ";
  result += binop->GetOp();
  result += " ";

  if (llvm::isa<Binop>(binop->GetRight())) {
    result += "(";
    result += Compute(binop->GetRight(), this);
    result += ")";
  } else {
    result += Compute(binop->GetRight(), this);
  }
}

void AstToStringVisitor::Visit(const Block *block) const noexcept {
  result = "{ ";
  for (const auto &expression : block->GetExpressions()) {
    result += Compute(expression, this);

    if (!llvm::isa<Conditional>(expression) && !llvm::isa<While>(expression)) {
      result += "; ";
    } else {
      result += " ";
    }
  }
  result += "}";
}

void AstToStringVisitor::Visit(const Boolean *boolean) const noexcept {
  if (boolean->GetValue()) {
    result = "true";
  }
  result = "false";
}

void AstToStringVisitor::Visit(const Conditional *conditional) const noexcept {
  result = "if (";
  result += Compute(conditional->GetTest(), this);
  result += ") ";
  result += Compute(conditional->GetFirst(), this);
  result += " else ";
  result += Compute(conditional->GetSecond(), this);
}

void AstToStringVisitor::Visit(const Dot *dot) const noexcept {
  result = Compute(dot->GetLeft(), this);
  result += ".";
  result += Compute(dot->GetRight(), this);
}

void AstToStringVisitor::Visit(const Function *function) const noexcept {
  result = "fn ";
  result += function->GetName();
  result += "(";

  std::size_t index  = 0;
  std::size_t length = function->GetArguments().size();
  for (const auto &argument : *function) {
    result += argument.first;
    result += ToString(argument.second);

    if (index < (length - 1)) {
      result += ", ";
    }
    index++;
  }

  result += ") ";
  result += Compute(function->GetBody(), this);
}

void AstToStringVisitor::Visit(const Integer *integer) const noexcept {
  result = std::to_string(integer->GetValue());
}

void AstToStringVisitor::Visit(const Nil *nil) const noexcept {
  assert(nil != nullptr);
  result = "nil";
}

void AstToStringVisitor::Visit(const Subscript *subscript) const noexcept {
  result = Compute(subscript->GetLeft(), this);
  result += "[";
  result += Compute(subscript->GetRight(), this);
  result += "]";
}

void AstToStringVisitor::Visit(const Tuple *tuple) const noexcept {
  result = "(";

  std::size_t index  = 0;
  std::size_t length = tuple->GetElements().size();
  for (const auto &element : *tuple) {
    result += Compute(element, this);

    if (index < (length - 1)) {
      result += ", ";
    }
    index++;
  }

  result += ")";
}

void AstToStringVisitor::Visit(const Unop *unop) const noexcept {
  result = unop->GetOp();
  result += Compute(unop->GetRight(), this);
}

void AstToStringVisitor::Visit(const Variable *variable) const noexcept {
  result = variable->GetSymbol();
}

void AstToStringVisitor::Visit(const While *loop) const noexcept {
  result = "while (";
  result += Compute(loop->GetTest(), this);
  result += ") ";
  result += Compute(loop->GetBody(), this);
}

[[nodiscard]] auto ToString(const Ast::Pointer &ast) noexcept -> std::string {
  AstToStringVisitor visitor;
  return visitor.Compute(ast, &visitor);
}
} // namespace pink
