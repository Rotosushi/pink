#include <cstdint>
#include <string>
#include <memory>

#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Constants.h"

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Object.hpp"

namespace pink {

Integer::Integer(const Location& loc)
  : Ast(Ast::Kind::Integer, loc), value(0)
{

}

Integer::Integer(const Location& loc, int64_t value)
  : Ast(Ast::Kind::Integer, loc), value(value)
{

}

bool Integer::classof(const Ast* ast)
{
  return ast->GetKind() == Ast::Kind::Integer;
}

std::shared_ptr<Integer> Integer::Clone()
{
  return std::make_shared<Integer>(loc, value);
}

std::string Integer::ToString()
{
  return std::to_string(value);
}

Judgement Integer::GetypeV(const Environment& env)
{
  return Judgement(std::make_shared<TypeLiteral>(loc, env.GetIntegerType());
}

Judgement Integer::Codegen(const Environment& env)
{
  return Judgement(std::make_shared<ValueLiteral>(loc, llvm::ConstantInt::get(env.GetIntegerType(), value)));
}

}
