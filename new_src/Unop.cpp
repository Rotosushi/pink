#include <string>
#include <memory>

#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Ast.hpp"
#include "UnopLiteralTable.hpp"
#include "UnopTable.hpp"


Unop::Unop(const Location& loc, InternedString op, std::shared_ptr<Ast> rhs)
  : Ast(Ast::Kind::Unop, loc), op(op), right(rhs)
{

}

bool Unop::classof(const Ast* ast)
{
  return ast->GetKind() == Ast::Kind::Unop;
}

std::shared_ptr<Unop> Unop::Clone()
{
  return std::make_shared<Unop>(op, right->Clone());
}

std::string Unop::ToString()
{
  std::string result;
  result += op;
  result += " " + right->ToString();
  return result;
}

Judgement Unop::GetypeV(const Environment& env)
{
  std::optional<std::shared_ptr<UnopLiteralTable>> ult = (env.GetUnopTable()).Lookup(op);

  if (ult)
  {
    Judgement right_jdgmt = right->Getype(env);

    if (!right_jdgmt)
    {
      return right_jdgmt;
    }

    const TypeLiteral* rtl = llvm::cast<TypeLiteral>(right_jdgmt.term.get());
    llvm::Type* rt = rtl->GetLiteral();
    std::optional<std::shared_ptr<UnopLiteral>> ul;

    if (llvm::PointerType* rpt = llvm::dyn_cast<llvm::PointerType>(rt))
    {
      // look for a unop of the unwrapped type,
      // to match up with our one free dereference rule.
      llvm::Type* rvt = rpt->getElementType();
      ul = ult->Lookup(rvt);
    }
    else
    {
      ul = ult->Lookup(rt);
    }



    if (ul)
    {
      return Judgement(std::make_shared<TypeLiteral>(loc, (*ul)->GetResultType());
    }
    else
    {
      std::string error_str;
      error_str += "unop [";
      error_str += op;
      error_str += "] does not have an implementation for the given value:[";
      error_str += rtl->ToString() + "]";
      return Judgement(error_str, loc);
    }
  }
  else
  {
    std::string error_str;
    error_str += "unop [";
    error_str += op;
    error_str += "] is not bound in the environment.";
    return Judgement(error_str, loc);
  }
}

Judgement Unop::Codegen(const Environment& env)
{
  std::optional<std::shared_ptr<UnopLiteralTable>> ult = (env.GetUnopTable()).Lookup(op);

  if (ult)
  {
    Judgement right_jdgmt = right->Codegen(env);

    if (!right_jdgmt)
    {
      return right_jdgmt;
    }

    // (r)ight (v)alue (l)iteral
    const ValueLiteral* rvl = llvm::cast<ValueLiteral*>(right_jdgmt.term.get());
    // (r)ight (v)alue
    llvm::Value* rv = nullptr;
    // (r)ight (t)ype
    llvm::Type* rt  = rv->getType();
    // (r)ight (p)ointer (t)ype
    llvm::PointerType* rpt = nullptr;
    // (u)nop (l)iteral
    std::optional<std::shared_ptr<UnopLiteral>> ul;

    // so, we return a reference from the variable term,
    // this means we need to unwrap this, using the one-free-dereference rule
    // if we can cast the right hand side to a Reference Type,
    // we lookup based upon the Referrent Type. since stored
    // pointers are accessed via two star pointers, the Referent Type
    // is itself a pointer, meaning we can tell the difference between
    // being passed a constant value, like 3, and being passed the
    // address of a local variable holding the value 3.
    if (rpt = llvm::dyn_cast<PointerType>(rt))
    {

      llvm::Type* rvt = rpt->getElementType();
      ul = ult->Lookup(rvt);

      if (!ul)
      {
        // we should never enter this segment of code,
        // as the Getype procedure would have noticed
        // this operators lack of an implementation for
        // the type before we get here.
        std::string error_str;
        error_str += "unop [";
        error_str += op;
        error_str += "] does not have an implementation for the given value:[";
        error_str += rvl->ToString() + "]";
        return Judgement(error_str, loc);
      }
      // the value literal needs unwrapped once.
      rv = env.BuildLoad(rvl->GetLiteral());
    }
    else
    {
      ul = ult->Lookup(rt);

      if (!ul)
      {
        // we should never enter this segment of code,
        // as the Getype procedure would have noticed
        // this operators lack of an implementation for
        // the type before we get here.
        std::string error_str;
        error_str += "unop [";
        error_str += op;
        error_str += "] does not have an implementation for the given value:[";
        error_str += rvl->ToString() + "]";
        return Judgement(error_str, loc);
      }
      // the value literal is holding the constant value.
      rv = rvl->GetLiteral();
    }
    // we have the right hand side fully computed, so
    // we can emit the code for the unary operation
    // itself. so all we have left is to call the
    // codegen procedure associated with this unop.
    return ul->Codegen(rv, env);
  }
  else
  {
    // we should not enter this segment of code,
    // the Getype procedure should have noticed
    // our lack of operator with the symbol bound
    // in our environment before we ever enter
    // this procedure.
    std::string error_str;
    error_str += "unop [";
    error_str += op;
    error_str += "] is not bound in the environment.";
    return Judgement(error_str, loc);
  }
}
