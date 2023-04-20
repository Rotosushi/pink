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
#include "ast/action/ToString.h"

#include "ast/All.h"

namespace pink {
class AstToStringVisitor
    : public ConstVisitorResult<AstToStringVisitor, const Ast::Pointer &, char>,
      public ConstAstVisitor {
  // #REASON: the visitor class is an implementation detail of
  // the ToString function, thus this class's reference
  // member is limited to calls of the function ToString, where
  // we know the lifetime of this reference is around for the
  // lifetime of this class. Thus the reference member cannot
  // cause a segmentation fault under normal evaluation.
  // NOLINTNEXTLINE
  std::string &buffer;

public:
  void Visit(const AddressOf *address_of) const noexcept override;
  void Visit(const Application *application) const noexcept override;
  void Visit(const Array *array) const noexcept override;
  void Visit(const Assignment *assignment) const noexcept override;
  void Visit(const Bind *bind) const noexcept override;
  void Visit(const Binop *binop) const noexcept override;
  void Visit(const Block *block) const noexcept override;
  void Visit(const Boolean *boolean) const noexcept override;
  void Visit(const IfThenElse *conditional) const noexcept override;
  void Visit(const Dot *dot) const noexcept override;
  void Visit(const Function *function) const noexcept override;
  void Visit(const Integer *integer) const noexcept override;
  void Visit(const Nil *nil) const noexcept override;
  void Visit(const Subscript *subscript) const noexcept override;
  void Visit(const Tuple *tuple) const noexcept override;
  void Visit(const Unop *unop) const noexcept override;
  void Visit(const ValueOf *value_of) const noexcept override;
  void Visit(const Variable *variable) const noexcept override;
  void Visit(const While *loop) const noexcept override;

  AstToStringVisitor(std::string &buffer) noexcept
      : buffer{buffer} {}
  ~AstToStringVisitor() override                               = default;
  AstToStringVisitor(const AstToStringVisitor &other) noexcept = default;
  AstToStringVisitor(AstToStringVisitor &&other) noexcept      = default;
  auto operator=(const AstToStringVisitor &other) noexcept
      -> AstToStringVisitor & = delete;
  auto operator=(AstToStringVisitor &&other) noexcept
      -> AstToStringVisitor & = delete;
};

void AstToStringVisitor::Visit(const AddressOf *address_of) const noexcept {
  buffer.append("&");
  Compute(address_of->GetRight(), this);
}

void AstToStringVisitor::Visit(const Application *application) const noexcept {
  Compute(application->GetCallee(), this);
  buffer.append("(");

  std::size_t index  = 0;
  std::size_t length = application->GetArguments().size();
  for (const auto &argument : application->GetArguments()) {
    Compute(argument, this);

    if (index < (length - 1)) {
      buffer.append(", ");
    }
    index++;
  }

  buffer.append(")");
}

void AstToStringVisitor::Visit(const Array *array) const noexcept {
  buffer.append("[");

  std::size_t index  = 0;
  std::size_t length = array->GetElements().size();
  for (const auto &element : array->GetElements()) {
    Compute(element, this);

    if (index < (length - 1)) {
      buffer.append(", ");
    }
    index++;
  }

  buffer.append("]");
}

void AstToStringVisitor::Visit(const Assignment *assignment) const noexcept {
  Compute(assignment->GetLeft(), this);
  buffer.append(" = ");
  Compute(assignment->GetRight(), this);
}

void AstToStringVisitor::Visit(const Bind *bind) const noexcept {
  buffer.append(bind->GetSymbol());
  buffer.append(" := ");
  Compute(bind->GetAffix(), this);
}

void AstToStringVisitor::Visit(const Binop *binop) const noexcept {
  if (llvm::isa<Binop>(binop->GetLeft())) {
    buffer.append("(");
    Compute(binop->GetLeft(), this);
    buffer.append(")");
  } else {
    Compute(binop->GetLeft(), this);
  }

  buffer.append(" ");
  buffer.append(ToString(binop->GetOp()));
  buffer.append(" ");

  if (llvm::isa<Binop>(binop->GetRight())) {
    buffer.append("(");
    Compute(binop->GetRight(), this);
    buffer.append(")");
  } else {
    Compute(binop->GetRight(), this);
  }
}

void AstToStringVisitor::Visit(const Block *block) const noexcept {
  buffer.append("{ ");
  for (const auto &expression : block->GetExpressions()) {
    Compute(expression, this);

    if (!llvm::isa<IfThenElse>(expression) && !llvm::isa<While>(expression)) {
      buffer.append("; ");
    } else {
      buffer.append(" ");
    }
  }
  buffer.append("}");
}

void AstToStringVisitor::Visit(const Boolean *boolean) const noexcept {
  if (boolean->GetValue()) {
    buffer.append("true");
  }
  buffer.append("false");
}

void AstToStringVisitor::Visit(const IfThenElse *conditional) const noexcept {
  buffer.append("if (");
  Compute(conditional->GetTest(), this);
  buffer.append(") ");
  Compute(conditional->GetFirst(), this);
  buffer.append(" else ");
  Compute(conditional->GetSecond(), this);
}

void AstToStringVisitor::Visit(const Dot *dot) const noexcept {
  Compute(dot->GetLeft(), this);
  buffer.append(".");
  Compute(dot->GetRight(), this);
}

void AstToStringVisitor::Visit(const Function *function) const noexcept {
  buffer.append("fn ");
  buffer.append(function->GetName());
  buffer.append("(");

  std::size_t index  = 0;
  std::size_t length = function->GetArguments().size();
  for (const auto &argument : *function) {
    buffer.append(argument.first);
    buffer.append(argument.second->ToString());

    if (index < (length - 1)) {
      buffer.append(", ");
    }
    index++;
  }

  buffer.append(") ");
  Compute(function->GetBody(), this);
}

void AstToStringVisitor::Visit(const Integer *integer) const noexcept {
  buffer.append(std::to_string(integer->GetValue()));
}

void AstToStringVisitor::Visit(const Nil *nil) const noexcept {
  assert(nil != nullptr);
  buffer.append("nil");
}

void AstToStringVisitor::Visit(const Subscript *subscript) const noexcept {
  result = Compute(subscript->GetLeft(), this);
  buffer.append("[");
  Compute(subscript->GetRight(), this);
  buffer.append("]");
}

void AstToStringVisitor::Visit(const Tuple *tuple) const noexcept {
  buffer.append("(");

  std::size_t index  = 0;
  std::size_t length = tuple->GetElements().size();
  for (const auto &element : *tuple) {
    Compute(element, this);

    if (index < (length - 1)) {
      buffer.append(", ");
    }
    index++;
  }

  buffer.append(")");
}

void AstToStringVisitor::Visit(const Unop *unop) const noexcept {
  buffer.append(ToString(unop->GetOp()));
  Compute(unop->GetRight(), this);
}

void AstToStringVisitor::Visit(const ValueOf *value_of) const noexcept {
  buffer.append("*");
  Compute(value_of->GetRight(), this);
}

void AstToStringVisitor::Visit(const Variable *variable) const noexcept {
  buffer.append(variable->GetSymbol());
}

void AstToStringVisitor::Visit(const While *loop) const noexcept {
  buffer.append("while (");
  Compute(loop->GetTest(), this);
  buffer.append(") ");
  Compute(loop->GetBody(), this);
}

[[nodiscard]] auto ToString(const Ast::Pointer &ast) noexcept -> std::string {
  std::string        buffer;
  AstToStringVisitor visitor{buffer};
  visitor.Compute(ast, &visitor);
  return buffer;
}
} // namespace pink
