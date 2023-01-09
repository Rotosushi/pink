#include "ast/AstToString.h"

#include "visitor/AstVisitor.h"

#include "support/DisableWarning.h"

#include <numeric> // std::accumulate

namespace pink {

class AstToStringVisitor : public ConstAstVisitor {
public:
  void Visit(const Application *application) const override {
    application->GetCallee()->Accept(this);

    buffer.emplace_back("(");

    size_t idx = 0;
    size_t len = application->GetArguments().size();
    for (const auto &arg : application->GetArguments()) {
      arg->Accept(this);

      if (idx < (len - 1)) {
        buffer.emplace_back(", ");
      }

      idx++;
    }
    buffer.emplace_back(")");
  }

  void Visit(const Array *array) const override {
    buffer.emplace_back("[");

    size_t idx = 0;
    auto len = array->GetMembers().size();
    for (const auto &memb : array->GetMembers()) {
      memb->Accept(this);

      if (idx < (len - 1)) {
        buffer.emplace_back(", ");
      }

      idx++;
    }

    buffer.emplace_back("]");
  }

  void Visit(const Assignment *assignment) const override {
    assignment->GetLeft()->Accept(this);
    buffer.emplace_back(" = ");
    assignment->GetRight()->Accept(this);
  }

  void Visit(const Bind *bind) const override {
    buffer.emplace_back(bind->GetSymbol());
    buffer.emplace_back(" := ");
    bind->GetAffix()->Accept(this);
  }

  void Visit(const Binop *binop) const override {
    const auto *left = binop->GetLeft();
    const auto *right = binop->GetRight();
    if (llvm::isa<Binop>(left)) {
      buffer.emplace_back("(");
      left->Accept(this);
      buffer.emplace_back(")");
    } else {
      left->Accept(this);
    }

    buffer.emplace_back(" ");
    buffer.emplace_back(binop->GetOp());
    buffer.emplace_back(" ");

    if (llvm::isa<Binop>(right)) {
      buffer.emplace_back("(");
      right->Accept(this);
      buffer.emplace_back(")");
    } else {
      right->Accept(this);
    }
  }

  void Visit(const Block *block) const override {
    buffer.emplace_back("{ ");

    for (const auto &stmt : block->statements) {
      stmt->Accept(this);
      buffer.emplace_back(";");
    }

    buffer.emplace_back(" }\n");
  }

  void Visit(const Boolean *boolean) const override {
    if (boolean->GetValue()) {
      buffer.emplace_back("true");
    } else {
      buffer.emplace_back("false");
    }
  }

  void Visit(const Conditional *conditional) const override {
    buffer.emplace_back("if ");
    conditional->GetTest()->Accept(this);
    buffer.emplace_back(" then ");
    conditional->GetFirst()->Accept(this);
    buffer.emplace_back(" else ");
    conditional->GetSecond()->Accept(this);
  }

  void Visit(const Dot *dot) const override {
    dot->GetLeft()->Accept(this);
    buffer.emplace_back(".");
    dot->GetRight()->Accept(this);
  }

  void Visit(const Function *function) const override {
    buffer.emplace_back("fn ");
    buffer.emplace_back(function->GetName());
    buffer.emplace_back("(");

    size_t idx = 0;
    size_t len = function->GetArguments().size();
    for (const auto &arg : function->GetArguments()) {
      buffer.emplace_back(arg.first);
      buffer.emplace_back(": ");
      buffer.emplace_back(arg.second->ToString());

      if (idx < (len - 1)) {
        buffer.emplace_back(", ");
      }
      idx++;
    }

    buffer.emplace_back(") {\n");
    function->GetBody()->Accept(this);
    buffer.emplace_back("\n}");
  }

  void Visit(const Integer *integer) const override {
    buffer.emplace_back(std::to_string(integer->GetValue()));
  }

  void Visit(const Nil *nil) const override {
    assert(nil != nullptr);
    buffer.emplace_back("nil");
  }

  void Visit(const Subscript *subscript) const override {
    subscript->GetLeft()->Accept(this);
    buffer.emplace_back("[");
    subscript->GetRight()->Accept(this);
    buffer.emplace_back("]");
  }

  void Visit(const Tuple *tuple) const override {
    buffer.emplace_back("(");

    size_t idx = 0;
    size_t length = tuple->GetMembers().size();
    for (const auto &member : tuple->GetMembers()) {
      member->Accept(this);

      if (idx < (length - 1)) {
        buffer.emplace_back(", ");
      }
      idx++;
    }

    buffer.emplace_back(")");
  }

  void Visit(const Unop *unop) const override {
    buffer.emplace_back(unop->GetOp());
    unop->GetRight()->Accept(this);
  }

  void Visit(const Variable *variable) const override {
    buffer.emplace_back(variable->GetSymbol());
  }

  void Visit(const While *loop) const override {
    buffer.emplace_back("while ");
    loop->GetTest()->Accept(this);
    buffer.emplace_back(" do ");
    loop->GetBody()->Accept(this);
  }

  AstToStringVisitor() = default;
  AstToStringVisitor(const AstToStringVisitor &other) = default;
  AstToStringVisitor(AstToStringVisitor &&other) = default;
  auto operator=(const AstToStringVisitor &other)
      -> AstToStringVisitor & = default;
  auto operator=(AstToStringVisitor &&other) -> AstToStringVisitor & = default;
  ~AstToStringVisitor() override = default;

  auto GetString() -> std::string {
    return std::accumulate(buffer.begin(), buffer.end(), std::string());
  }

private:
  mutable std::vector<std::string> buffer;
};

auto AstToString(const Ast *ast) -> std::string {
  AstToStringVisitor visitor;

  ast->Accept(&visitor);

  return visitor.GetString();
}

} // namespace pink