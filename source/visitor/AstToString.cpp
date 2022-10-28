#include "visitor/AstToString.h"

#include "support/DisableWarning.h"

namespace pink {
void AstToString::Visit(const Application *application) const {
  std::string result;

  result += Compute(application->GetCallee());
  result += "(";

  size_t idx = 0;
  size_t len = application->GetArguments().size();
  for (const auto &arg : *application) {
    result += Compute(arg.get());

    if (idx < (len - 1)) {
      result += ", ";
    }

    idx++;
  }
  result += ")";
  Return(result);
}

void AstToString::Visit(const Array *array) const {
  std::string result;
  result += "[";

  size_t idx = 0;
  auto len = array->GetMembers().size();
  for (const auto &memb : *array) {
    result += Compute(memb.get());

    if (idx < (len - 1)) {
      result += ", ";
    }

    idx++;
  }

  result += "]";
  Return(result);
}

void AstToString::Visit(const Assignment *assignment) const {
  std::string result =
      Compute(assignment->GetLeft()) + " = " + Compute(assignment->GetRight());
  Return(result);
}

void AstToString::Visit(const Bind *bind) const {
  Return(std::string(bind->GetSymbol()) += " := " + Compute(bind->GetAffix()));
}

void AstToString::Visit(const Binop *binop) const {
  std::string result;
  const auto *left = binop->GetLeft();
  const auto *right = binop->GetRight();
  if (llvm::isa<Binop>(left)) {
    result += "(" + Compute(left) + ")";
  } else {
    result += Compute(left);
  }

  result += " " + std::string(binop->GetOp()) + " ";

  if (llvm::isa<Binop>(right)) {
    result += "(" + Compute(right) + ")";
  } else {
    result += Compute(right);
  }

  Return(result);
}

void AstToString::Visit(const Block *block) const {
  std::string result("{ ");

  for (const auto &stmt : *block) {
    result += Compute(stmt.get()) + ";\n";
  }

  result += " }";

  Return(result);
}

void AstToString::Visit(const Bool *boolean) const {
  if (boolean->GetValue()) {
    Return("true");
  } else {
    Return("false");
  }
}

void AstToString::Visit(const Conditional *conditional) const {
  const auto *test = conditional->GetTest();
  const auto *first = conditional->GetFirst();
  const auto *second = conditional->GetSecond();
  std::string result("if ");
  result += Compute(test);
  result += " then ";
  result += Compute(first);
  result += " else ";
  result += Compute(second);
  Return(result);
}

void AstToString::Visit(const Dot *dot) const {
  Return(Compute(dot->GetLeft()) + "." + Compute(dot->GetRight()));
}

void AstToString::Visit(const Function *function) const {
  const auto *name = function->GetName();
  const auto &arguments = function->GetArguments();
  const auto *body = function->GetBody();
  std::string result(std::string("fn ") + name + std::string(" ("));

  size_t idx = 0;
  size_t len = arguments.size();
  for (const auto &arg : *function) {
    result += arg.first;
    result += ": " + arg.second->ToString();

    if (idx < (len - 1)) {
      result += ", ";
    }
    idx++;
  }

  result += ") {\n" + Compute(body) + "\n}";
  Return(result);
}

void AstToString::Visit(const Int *integer) const {
  Return(std::to_string(integer->GetValue()));
}

NOWARN(
    "-Wunused-parameter",
    void AstToString::Visit(const Nil *nil) const { Return("nil"); } // NOLINT
)

void AstToString::Visit(const Subscript *subscript) const {
  Return(Compute(subscript->GetLeft()) + "[" + Compute(subscript->GetRight()) +
         "]");
}

void AstToString::Visit(const Tuple *tuple) const {
  std::string result = "(";
  size_t idx = 0;
  size_t length = tuple->GetMembers().size();

  for (const auto &member : *tuple) {
    result += Compute(member.get());

    if (idx < (length - 1)) {
      result += ", ";
    }
    idx++;
  }
  result += ")";

  Return(result);
}

void AstToString::Visit(const Unop *unop) const {
  Return(unop->GetOp() + Compute(unop->GetRight()));
}

void AstToString::Visit(const Variable *variable) const {
  Return({variable->GetSymbol()});
}

void AstToString::Visit(const While *loop) const {
  std::string result("while ");
  result += Compute(loop->GetTest());
  result += " do ";
  result += Compute(loop->GetBody());
  Return(result);
}

} // namespace pink