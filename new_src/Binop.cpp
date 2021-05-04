#include <string>
#include <memory>
#include <optional>

#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Ast.hpp"
#include "Binop.hpp"

Binop::Binop(const Location& loc, InternedString op, std::shared_ptr<Ast> left, std::shared_ptr<Ast> right)
  : Ast(loc), op(op), left(left), right(right)
{

}

std::shared_ptr<Binop> Binop::Clone()
{
  return std::make_shared<Binop>(loc, op, left->Clone(), right->Clone());
}

std::string Binop::ToString()
{
  std::string result;
  result += left->ToString();
  result +=  " " + op + " ";
  result += right->ToString();
  return result;
}

Judgement Binop::Getype(const Environment& env)
{
  const BinopTable& binops = env.GetBoundBinops();

  // find the binop
  std::optional<std::shared_ptr<BinopCodegenTable>> bound_binop = binops.Lookup(op);

  if (bound_binop)
  {

  }
  else
  {

  }
}

Judgement Binop::Codegen(const Environment& env)
{

}
