#include <memory>
#include <optional>
#include <vector>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"

#include "Judgement.hpp"
#include "Ast.hpp"
#include "Object.hpp"
#include "TypeLiteral.hpp"
#include "ValueLiteral.hpp"
#include "Environment.hpp"
#include "Kernel.hpp"

namespace pink {

Judgement UnopBangNegateBoolean(std::shared_ptr<Ast>& rhs, const Environment& env)
{
  // steps for this algorithm broadly:
  // did we get a value we can work with correctly?
  // if yes, emit operation, if no emit error.
}

Judgement UnopMinusNegateInteger(std::shared_ptr<Ast>& rhs, const Environment& env)
{

}

/*
  programmers don't do anything -to- nil values
  except inspection/reading

  integer negation (- Int)
  boolean negation (! Bool)

*/
void RegisterPrimitiveUnops(const Environment& env)
{



}


/*
// later
  bitwise // which should be made to work on any type with size one word or smaller.
    and  (Word && Word)
    or   (Word || Word)
    xor  (Word ^^ Word)
    nand (Word !&& Word)
    nor  (Word !|| Word)
    xnor (Word !^^ Word)

// right now:
  assignment
    Int  <- Int
    Bool <- Bool

  integer
    add (Int + Int)
    sub (Int - Int)
    mul (Int * Int)
    div (Int / Int)
    mod (Int % Int)

  boolean
    and  (Bool & Bool)
    or   (Bool | Bool)
    xor  (Bool ^ Bool)
    nand (Bool !& Bool)
    nor  (Bool !| Bool)
    xnor (Bool !^ Bool)

  // more types, overloading the functions for those types.
  // Integer types of different, specific sizes (8/16/32/64)
  // with arithmetic working on and between them all.
  //
*/


Judgement BinopPlusAddInteger(std::shared_ptr<Ast>& l, std::shared_ptr<Ast>& r, const Environment& env)
{
  if (ValueLiteral* lvl = llvm::dyn_cast<ValueLiteral>(l.get()))
  {
    if (ValueLiteral* rvl = llvm::dyn_cast<ValueLiteral>(r.get()))
    {

    }
    else
    {
      FatalError("Passed right term isn't a ValueLiteral ~> " + r->ToString(), __FILE__, __LINE__);
    }
  }
  else
  {
    FatalError("Passed Left term isn't a ValueLiteral ~> " + l->ToString(), __FILE__, __LINE__);
  }
}



Judgement BinopHyphenMinusInteger(std::shared_ptr<Ast>& l, std::shared_ptr<Ast>& r, const Environment& env)
{
  if (ValueLiteral* lvl = llvm::dyn_cast<ValueLiteral>(l.get()))
  {
    if (ValueLiteral* rvl = llvm::dyn_cast<ValueLiteral>(r.get()))
    {

    }
    else
    {
      FatalError("Passed right term isn't a ValueLiteral ~> " + r->ToString(), __FILE__, __LINE__);
    }
  }
  else
  {
    FatalError("Passed Left term isn't a ValueLiteral ~> " + l->ToString(), __FILE__, __LINE__);
  }
}



Judgement BinopStarMultiplyInteger(std::shared_ptr<Ast>& l, std::shared_ptr<Ast>& r, const Environment& env)
{
  if (ValueLiteral* lvl = llvm::dyn_cast<ValueLiteral>(l.get()))
  {
    if (ValueLiteral* rvl = llvm::dyn_cast<ValueLiteral>(r.get()))
    {

    }
    else
    {
      FatalError("Passed right term isn't a ValueLiteral ~> " + r->ToString(), __FILE__, __LINE__);
    }
  }
  else
  {
    FatalError("Passed Left term isn't a ValueLiteral ~> " + l->ToString(), __FILE__, __LINE__);
  }
}



Judgement BinopFSlashDivideInteger(std::shared_ptr<Ast>& l, std::shared_ptr<Ast>& r, const Environment& env)
{
  if (ValueLiteral* lvl = llvm::dyn_cast<ValueLiteral>(l.get()))
  {
    if (ValueLiteral* rvl = llvm::dyn_cast<ValueLiteral>(r.get()))
    {

    }
    else
    {
      FatalError("Passed right term isn't a ValueLiteral ~> " + r->ToString(), __FILE__, __LINE__);
    }
  }
  else
  {
    FatalError("Passed Left term isn't a ValueLiteral ~> " + l->ToString(), __FILE__, __LINE__);
  }
}



Judgement BinopAmpersandAndBoolean(std::shared_ptr<Ast>& l, std::shared_ptr<Ast>& r, const Environment& env)
{
  if (ValueLiteral* lvl = llvm::dyn_cast<ValueLiteral>(l.get()))
  {
    if (ValueLiteral* rvl = llvm::dyn_cast<ValueLiteral>(r.get()))
    {

    }
    else
    {
      FatalError("Passed right term isn't a ValueLiteral ~> " + r->ToString(), __FILE__, __LINE__);
    }
  }
  else
  {
    FatalError("Passed Left term isn't a ValueLiteral ~> " + l->ToString(), __FILE__, __LINE__);
  }
}



Judgement BinopBarOrBoolean(std::shared_ptr<Ast>& l, std::shared_ptr<Ast>& r, const Environment& env)
{
  if (ValueLiteral* lvl = llvm::dyn_cast<ValueLiteral>(l.get()))
  {
    if (ValueLiteral* rvl = llvm::dyn_cast<ValueLiteral>(r.get()))
    {

    }
    else
    {
      FatalError("Passed right term isn't a ValueLiteral ~> " + r->ToString(), __FILE__, __LINE__);
    }
  }
  else
  {
    FatalError("Passed Left term isn't a ValueLiteral ~> " + l->ToString(), __FILE__, __LINE__);
  }
}



Judgement BinopCaretXorBoolean(std::shared_ptr<Ast>& l, std::shared_ptr<Ast>& r, const Environment& env)
{
  if (ValueLiteral* lvl = llvm::dyn_cast<ValueLiteral>(l.get()))
  {
    if (ValueLiteral* rvl = llvm::dyn_cast<ValueLiteral>(r.get()))
    {

    }
    else
    {
      FatalError("Passed right term isn't a ValueLiteral ~> " + r->ToString(), __FILE__, __LINE__);
    }
  }
  else
  {
    FatalError("Passed Left term isn't a ValueLiteral ~> " + l->ToString(), __FILE__, __LINE__);
  }
}



Judgement BinopBangAmpersandNandBoolean(std::shared_ptr<ast>& l, std::shared_ptr<Ast>& r, const Environment& env)
{
  if (ValueLiteral* lvl = llvm::dyn_cast<ValueLiteral>(l.get()))
  {
    if (ValueLiteral* rvl = llvm::dyn_cast<ValueLiteral>(r.get()))
    {

    }
    else
    {
      FatalError("Passed right term isn't a ValueLiteral ~> " + r->ToString(), __FILE__, __LINE__);
    }
  }
  else
  {
    FatalError("Passed Left term isn't a ValueLiteral ~> " + l->ToString(), __FILE__, __LINE__);
  }
}



Judgement BinopBangBarNorBoolean(std::shared_ptr<Ast>& l, std::shared_ptr<Ast>& r, const Environment& env)
{
  if (ValueLiteral* lvl = llvm::dyn_cast<ValueLiteral>(l.get()))
  {
    if (ValueLiteral* rvl = llvm::dyn_cast<ValueLiteral>(r.get()))
    {

    }
    else
    {
      FatalError("Passed right term isn't a ValueLiteral ~> " + r->ToString(), __FILE__, __LINE__);
    }
  }
  else
  {
    FatalError("Passed Left term isn't a ValueLiteral ~> " + l->ToString(), __FILE__, __LINE__);
  }
}



Judgement BinopBangCaretXnorBoolean(std::shared_ptr<Ast>& l, std::shared_ptr<Ast>& r, const Environment& env)
{
  if (ValueLiteral* lvl = llvm::dyn_cast<ValueLiteral>(l.get()))
  {
    if (ValueLiteral* rvl = llvm::dyn_cast<ValueLiteral>(r.get()))
    {

    }
    else
    {
      FatalError("Passed right term isn't a ValueLiteral ~> " + r->ToString(), __FILE__, __LINE__);
    }
  }
  else
  {
    FatalError("Passed Left term isn't a ValueLiteral ~> " + l->ToString(), __FILE__, __LINE__);
  }
}



void RegisterPrimitiveBinops(const Environment& env)
{

}




















}



















// --
