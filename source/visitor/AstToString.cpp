#include "visitor/AstToString.h"

#include "support/DisableWarning.h"

namespace pink {
void AstToString::Visit(Application *application) {
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

void AstToString::Visit(Array *array) {
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

void AstToString::Visit(Assignment *assignment) {
  std::string result =
      Compute(assignment->GetLeft()) + " = " + Compute(assignment->GetRight());
  Return(result);
}

void AstToString::Visit(Bind *bind) {
  Return(std::string(bind->GetSymbol()) += " := " + Compute(bind->GetAffix()));
}

void AstToString::Visit(Binop *binop) {
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

void AstToString::Visit(Block *block) {
  std::string result("{ ");

  for (const auto &stmt : *block) {
    result += Compute(stmt.get()) + ";\n";
  }

  result += " }";

  Return(result);
}

void AstToString::Visit(Bool *boolean) {
  if (boolean->GetValue()) {
    Return("true");
  } else {
    Return("false");
  }
}

void AstToString::Visit(Conditional *conditional) {
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

void AstToString::Visit(Dot *dot) {
  Return(Compute(dot->GetLeft()) + "." + Compute(dot->GetRight()));
}

void AstToString::Visit(Function *function) {
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

void AstToString::Visit(Int *integer) {
  Return(std::to_string(integer->GetValue()));
}

NOWARN(
    "-Wunused-parameter",
    void AstToString::Visit(Nil *nil) { Return("nil"); } // NOLINT
)

void AstToString::Visit(Tuple *tuple) {
  std::string result("(");
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

void AstToString::Visit(Unop *unop) {
  Return(unop->GetOp() + Compute(unop->GetRight()));
}

void AstToString::Visit(Variable *variable) { Return({variable->GetSymbol()}); }

void AstToString::Visit(While *loop) {
  std::string result("while ");
  result += Compute(loop->GetTest());
  result += " do ";
  result += Compute(loop->GetBody());
  Return(result);
}

} // namespace pink