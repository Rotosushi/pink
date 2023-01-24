

#include "ast/action/ToString.h"
#include "type/action/ToString.h"

#include "ast/All.h"

namespace pink {
void AstToString::Visit(const Application *application) const noexcept {
  result = Compute(application->GetCallee());
  result += "(";

  std::size_t index = 0;
  std::size_t length = application->GetArguments().size();
  for (const auto &argument : application->GetArguments()) {
    result += Compute(argument);

    if (index < (length - 1)) {
      result += ", ";
    }
    index++;
  }

  result += ")";
}

void AstToString::Visit(const Array *array) const noexcept {
  result = "[";

  std::size_t index = 0;
  std::size_t length = array->GetElements().size();
  for (const auto &element : array->GetElements()) {
    result += Compute(element);

    if (index < (length - 1)) {
      result += ", ";
    }
    index++;
  }

  result += "]";
}

void AstToString::Visit(const Assignment *assignment) const noexcept {
  result = Compute(assignment->GetLeft());
  result += " = ";
  result += Compute(assignment->GetRight());
}

void AstToString::Visit(const Bind *bind) const noexcept {
  result = bind->GetSymbol();
  result += " := ";
  result += Compute(bind->GetAffix());
}

void AstToString::Visit(const Binop *binop) const noexcept {
  if (llvm::isa<Binop>(binop->GetLeft())) {
    result += "(";
    result += Compute(binop->GetLeft());
    result += ")";
  } else {
    result += Compute(binop->GetLeft());
  }

  result += " ";
  result += binop->GetOp();
  result += " ";

  if (llvm::isa<Binop>(binop->GetRight())) {
    result += "(";
    result += Compute(binop->GetRight());
    result += ")";
  } else {
    result += Compute(binop->GetRight());
  }
}

void AstToString::Visit(const Block *block) const noexcept {
  result = "{ ";
  for (const auto &expression : block->GetExpressions()) {
    result += Compute(expression);

    if (!llvm::isa<Conditional>(expression) && !llvm::isa<While>(expression)) {
      result += "; ";
    } else {
      result += " ";
    }
  }
  result += "}";
}

void AstToString::Visit(const Boolean *boolean) const noexcept {
  if (boolean->GetValue()) {
    result = "true";
  }
  result = "false";
}

void AstToString::Visit(const Conditional *conditional) const noexcept {
  result = "if (";
  result += Compute(conditional->GetTest());
  result += ") ";
  result += Compute(conditional->GetFirst());
  result += " else ";
  result += Compute(conditional->GetSecond());
}

void AstToString::Visit(const Dot *dot) const noexcept {
  result = Compute(dot->GetLeft());
  result += ".";
  result += Compute(dot->GetRight());
}

void AstToString::Visit(const Function *function) const noexcept {
  result = "fn ";
  result += function->GetName();
  result += "(";

  std::size_t index = 0;
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
  result += Compute(function->GetBody());
}

void AstToString::Visit(const Integer *integer) const noexcept {
  result = std::to_string(integer->GetValue());
}

void AstToString::Visit(const Nil *nil) const noexcept {
  assert(nil != nullptr);
  result = "nil";
}

void AstToString::Visit(const Subscript *subscript) const noexcept {
  result = Compute(subscript->GetLeft());
  result += "[";
  result += Compute(subscript->GetRight());
  result += "]";
}

void AstToString::Visit(const Tuple *tuple) const noexcept {
  result = "(";

  std::size_t index = 0;
  std::size_t length = tuple->GetElements().size();
  for (const auto &element : *tuple) {
    result += Compute(element);

    if (index < (length - 1)) {
      result += ", ";
    }
    index++;
  }

  result += ")";
}

void AstToString::Visit(const Unop *unop) const noexcept {
  result = unop->GetOp();
  result += Compute(unop->GetRight());
}

void AstToString::Visit(const Variable *variable) const noexcept {
  result = variable->GetSymbol();
}

void AstToString::Visit(const While *loop) const noexcept {
  result = "while (";
  result += Compute(loop->GetTest());
  result += ") ";
  result += Compute(loop->GetBody());
}

[[nodiscard]] auto ToString(const Ast::Pointer &ast) noexcept -> std::string {
  return AstToString::Compute(ast);
}
} // namespace pink
