#include "llvm/Support/raw_ostream.h"

#include "ValueLiteral.hpp"

namespace pink {

ValueLiteral::ValueLiteral(const Location& loc, llvm::Value* literal)
  : Ast(Ast::Kind::ValueLiteral, loc), literal(literal)
{

}

std::shared_ptr<ValueLiteral> ValueLiteral::Clone()
{
  return std::make_shared<ValueLiteral>(loc, literal);
}

std::string ValueLiteral::ToString()
{
  std::string buf;
  llvm::raw_string_ostream hdl(buf);
  literal->print(hdl);
  return hdl.str();
}

bool ValueLiteral::classof(const Ast* ast)
{
  return ast->GetKind() == Ast::Kind::ValueLiteral;
}

llvm::Value* ValueLiteral::GetLiteral()
{
  return literal;
}

Judgement ValueLiteral::GetypeV(const Environment& env)
{
  return Judgement(std::make_shared<TypeLiteral>(loc, literal->getType()));
}

Judgement ValueLiteral::Codegen(const Environment& env)
{
  return Judgement(std::make_shared<ValueLiteral>(loc, literal));
}

}
