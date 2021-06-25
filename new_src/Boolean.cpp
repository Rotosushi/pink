#include <string>
#include <memory>

#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Constants.h"

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Object.hpp"
#include "TypeLiteral.hpp"
#include "ValueLiteral.hpp"
#include "Boolean.hpp"

namespace pink {

Boolean::Boolean(const Location& loc)
  : Ast(Ast::Kind::Boolean, loc), value(false)
{

}

Boolean::Boolean(const Location& loc, bool val)
  : Ast(Ast::Kind::Boolean, loc), value(val)
{

}

bool Boolean::classof(const Ast* ast)
{
  return ast->GetKind() == Ast::Kind::Boolean;
}

std::shared_ptr<Boolean> Boolean::Clone()
{
  return std::make_shared<Boolean>(loc, value);
}

std::string Boolean::ToString()
{
  if (value)
  {
    return std::string("true");
  }
  else
  {
    return std::string("false");
  }
}

Judgement Boolean::GetypeV(const Environment& env)
{
  return std::make_shared<TypeLiteral>(loc, env.GetBooleanType());
}

Judgement Boolean::Codegen(const Environment& env)
{
  Judgement result;
  llvm::Type* BooleanType = env.GetBooleanType();

  if (value)
  {
    result = Judgement(std::make_shared<ValueLiteral>(loc, llvm::ConstantInt::get(BooleanType, 1)));
  }
  else
  {
    result = Judgement(std::make_shared<ValueLiteral>(loc, llvm::ConstantInt::get(BooleanType, 0)));
  }

  return result;
}

}
