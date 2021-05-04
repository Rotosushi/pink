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

Integer::Integer(const Location& loc, int64_t value)
  : Object(loc), value(value)
{

}

std::shared_ptr<Integer> Integer::Clone()
{
  return std::make_shared<Integer>(loc, value);
}

std::string Integer::ToString()
{
  return std::to_string(value);
}

Judgement Integer::Getype(const Environment& env)
{
  return Judgement(std::make_shared<TypeLiteral>(loc, llvm::Type::getInt64Ty(env.GetContext())));
}

Judgement Integer::Codegen(const Environment& env)
{
  llvm::Type* IntegerType = llvm::Type::getInt64Ty(env.GetContext());
  return Judgement(std::make_shared<ValueLiteral>(loc, llvm::ConstantInt::get(IntegerType, value)));
}
