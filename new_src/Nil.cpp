#include <string>
#include <memory>

#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Object.hpp"

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
  llvm::Type* NilType = llvm::Type::getInt1Ty(env.GetContext());
  return Judgement(std::make_shared<TypeLiteral>(loc, NilType));
}

Judgement Nil::Codegen(const Environment& env)
{
  llvm::Type* NilType = llvm::Type::getInt1Ty(env.GetContext());
  return Judgement(std::make_shared<ValueLiteral>(loc, llvm::ConstantInt::get(NilType, 0)));
}
