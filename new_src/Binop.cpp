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

namespace pink {

Binop::Binop(const Location& loc, InternedString op, std::shared_ptr<Ast> left, std::shared_ptr<Ast> right)
  : Ast(Ast::Kind::Binop, loc), op(op), left(left), right(right)
{

}

bool Binop::classof(const Ast* ast)
{
  return ast->GetKind() == Ast::Kind::Binop;
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

Judgement Binop::GetypeV(const Environment& env)
{
  const BinopTable& binops = env.GetBinopTable();

  // find the binop
  std::optional<std::shared_ptr<BinopCodegenTable>> bound_binop = binops.Lookup(op);

  if (bound_binop)
  {
    // calls of getype return valid TypeLiterals
    // or valid Errors.
    Judgement left_type = left->Getype(env);

    if (!left_type)
    {
      return left_type;
    }

    Judgement right_type = right->Getype(env);

    if (!right_type)
    {
      return right_type;
    }

    std::shared_ptr<TypeLiteral> ltl = dynamic_cast<TypeLiteral>(left_type.term);
    std::shared_ptr<TypeLiteral> rtl = dynamic_cast<TypeLiteral>(right_type.term);
    llvm::Type* lt = ltl->GetLiteral();
    llvm::Type* rt = rtl->GetLiteral();

    std::optional<std::shared_ptr<BinopCodegen>> binop = bound_binop.Lookup(lt, rt);

    if (binop)
    {
      return Judgement(std::make_shared<TypeLiteral>(loc, (*binop)->GetResultType()));
    }
    else
    {
      std::string error_str;
      error_str += "binop [";
      error_str += op;
      error_str += "] doesn't have an implementation for the given Types. left:[";
      error_str += ltl->ToString() + "] right:[";
      error_str += rtl->ToString() + "]";
      return Judgement(Error(error_str, loc));
    }

  }
  else
  {
    std::string error_str;
    error_str += "binop :[" + std::string(op) + "] ";
    error_str += "not bound in environment.";
    return Judgement(Error(error_str, loc));
  }
}

Judgement Binop::Codegen(const Environment& env)
{
  const BinopTable& binops = env.GetBinopTable();

  // find the binop
  std::optional<std::shared_ptr<BinopCodegenTable>> bound_binop = binops.Lookup(op);

  if (bound_binop)
  {
    // generate the left and right hand sides.
    Judgement left_jdgmt = left->Codegen(env);

    if (!left_jdgmt)
    {
      return left_jdgmt;
    }

    Judgement right_jdgmt = right->Codegen(env);

    if (!right_jdgmt)
    {
      return right_jdgmt;
    }

    //
    llvm::Value* left_value = nullptr;
    llvm::Value* right_value = nullptr;

    std::shared_ptr<ValueLiteral> lvl = dynamic_cast<ValueLiteral>(left_jdgmt.term);
    std::shared_ptr<ValueLiteral> rvl = dynamic_cast<ValueLiteral>(right_jdgmt.term);
    llvm::Type* lt = (lvl->GetLiteral())->getType();
    llvm::Type* rt = (rvl->GetLiteral())->getType();

    std::optional<std::shared_ptr<BinopCodegen>> binop = bound_binop.Lookup(lt, rt);

    if (binop)
    {
      return binop->Codegen(lvl, rvl, env);
    }
    else
    {
      std::string error_str;
      error_str += "binop doesn't have an implementation for the given values. left:[";
      error_str += ltl->ToString() + "] right:[";
      error_str += rtl->ToString() + "]";
      return Judgement(Error(error_str, loc));
    }

  }
  else
  {
    std::string error_str;
    error_str += "binop :[" + std::string(op) + "] ";
    error_str += "not bound in environment.";
    return Judgement(Error(error_str, loc));
  }
}


}
