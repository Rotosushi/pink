#include <string>
#include <memory>

#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Object.hpp"

namespace pink {

Nil::Nil(const Location& loc)
  : Ast(Ast::Kind::Nil, loc)
{

}

bool Nil::classof(const Ast* ast)
{
  return ast->GetKind() == Ast::Kind::Nil;
}

std::shared_ptr<Nil> Nil::Clone()
{
  return std::make_shared<Nil>(loc);
}

std::string Nil::ToString()
{
  return std::string("Nil");
}

Judgement Nil::Getype(const Environment& env)
{
  return Judgement(std::make_shared<TypeLiteral>(loc, env.GetNilType()));
}

Judgement Nil::Codegen(const Environment& env)
{
  return Judgement(std::make_shared<ValueLiteral>(loc, llvm::ConstantInt::get(env.GetNilType(), 0)));
}

}
