#include <string>
#include <memory>

#include "llvm/IR/Value.h"

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Object.hpp"
#include "ValueLiteral.hpp"


ValueLiteral::ValueLiteral(const Location& loc, llvm::Value* literal)
  : Object(loc), literal(literal)
{

}

std::shared_ptr<ValueLiteral> ValueLiteral::Clone()
{
  return std::make_shared<ValueLiteral>(loc, literal);
}

std::string ValueLiteral::ToString()
{
  std::string result;
  llvm::raw_string_ostream handle(result);
  literal->print(handle);
  return handle.str();
}

Judgement ValueLiteral::Getype(const Environment& env)
{
  return std::make_shared<TypeLiteral>(loc, literal->getType());
}

Judgement ValueLiteral::Codegen(const Environment& env)
{
  return std::make_shared<ValueLiteral>(loc, literal);
}
