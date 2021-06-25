

#include "Assignment.hpp"

namespace pink {

Assignment::Assignment(const Location& loc, std::shared_ptr<Ast> l, std::shared_ptr<Ast> r)
  : Ast(Ast::Kind::Assignment, loc), left(l), right(r)
{

}

std::shared_ptr<Assignment> Assignment::Clone()
{
  return std::make_shared<Assignment>(loc, l, r);
}

std::string Assignment::ToString()
{
  std::string result;
  result += left->ToString();
  result += " <- ";
  result += right->ToString();
  return
}

bool Assignment::classof(Ast* ast)
{
  return ast->GetKind() == Ast::Kind::Assignment;
}


Judgement Assignment::GetypeV(const Environment& env)
{

}

Judgement Assignment::Codegen(const Environment& env)
{

}

}
