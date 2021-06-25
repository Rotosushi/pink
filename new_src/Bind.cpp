#include <string>
#include <memory>
#include <optional>


#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Instructions.h"



#include "Location.hpp"
#include "StringInterner.hpp"
#include "Ast.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Bind.hpp"

namespace pink {

Bind::Bind(const Location& loc, InternedString& name, std::shared_ptr<Ast> rhs)
  : Ast(Ast::Kind::Bind, loc), name(name), right(rhs)
{

}

bool Bind::classof(const Ast* ast)
{
  return ast->GetKind() == Ast::Kind::Bind;
}


std::shared_ptr<Bind> Bind::Clone()
{
  return std::make_shared(Bind(loc, name, right->Clone()));
}

std::string Bind::ToString()
{
  std::string result;
  result += InternedString;
  result += " := " + right->ToString();
  return result;
}

/*
  Getype needs to accomplish one thing,
  check that this bind term isn't introducing
  a name conflict.
  i.e. we outright disallow the statement
  \x:Int => x := 1; x + x

*/
Judgement Bind::GetypeV(const Environment& env)
{
  std::optional<std::shared_ptr<Ast>> bound = (env.GetSymbolTable())->LookupLocal(name);
  if (bound)
  {
    std::string errtxt;
    errtxt += "[" + name + std::string("] already bound in local scope to: [");
    errtxt += (*bound)->ToString() + "]";
  }
  else
  {
    Judgement rhsjdgmt = right->Getype(env);
    return rhsjdgmt;
  }
}

/*
  -) emit the right hand side of the bind expression,
  -) we were given an AllocaInst by this so,
     we could just bind that to the Variable in
     the symboltable right? we could, but we won't
     just yet out of an abundance of caution.
*/
Judgement Bind::Codegen(const Environment& env)
{
  // compute the rhs of the bind expression
  // this will return us a temporary allocation
  // containing the value of the rhs expression
  Judgement rhsjdgmt = right->Codegen(env);

  if (rhsjdgmt.IsAnError())
    return rhsjdgmt;

  // local alias.
  const llvm::IRBuilder& bldr = env.GetIRBuilder();
  const llvm::DataLayout& dl  = end.GetDataLayout();

  // okay okay, so i think we are actually going to
  // pass value literals around, and only build an
  // alloca during a bind. the symbol table still
  // binds AllocaInst, but Variable builds a
  // load of that memory, taking into account that
  // some types are too big to fit inside a single
  // register. I think to perform the conversion
  // from the AllocaInst to a Value*, for a type
  // that does fit in the largest integer type width,
  // we can generate a store. however, if the
  // type is too big to fit in a register, LLVM
  // recommends building loads or stores of individual
  // fields of the type.
  // https://llvm.org/docs/Frontend/PerformanceTips.html#avoid-loads-and-stores-of-large-aggregate-type
  auto         value_literal = dynamic_cast<std::shared_ptr<ValueLiteral>>(rhsjdgmt.term);
  llvm::Value* rhs_literal   = value_literal->GetLiteral();
  llvm::Type*  rhs_type      = rhs_literal->getType();


  // we need to tell what kind of allocation we want to build,
  // a global variable, or a local one. the global scoped IRBuilder doesn't
  // have an insertion point. (we explicitly Clear it on creation.)
  // so when we go to retrieve an insertion block, if we are
  // given a nullptr, we know we are binding in a global context.
  // if we are given a basic block, then we know we are binding in a
  // local context. (we could also check that our current scopeSet
  // was equal to 1, which is the global scope.)
  llvm::BasicBlock* insertion_block = bldr.GetInsertBlock();

  if (insertion_block != nullptr)
  {
    // if we have a basic block, we are within
    // a function definition, and that means we
    // want an alloca instruction.
    // recall that LLVM want's all local variables
    // allocated at the beginning of the procedures
    // local block for optimizations.
    // https://llvm.org/docs/Frontend/PerformanceTips.html#use-of-allocas
    llvm::Function* function = insertion_block->getParent();

    // we have to insert into the top of the procedure,
    // but we want to store -after- we compute the result,
    // which is presumably the call the right->Codegen above.
    // (i.e. we want to insert the store after we insert the
    //   initializer instructions, because the initializer instructions
    //   compute the value we want to store.)
    llvm::AllocaInst* local_storage = env.BuildLocalAlloca(rhs_type, name, function);
    // we have to contend with the fact that the size might be larger than
    // we can efficiently use the store instruction with.
    // getTypeAllocSize has an *InBits variant.
    // which tells me that the default is InBytes.
    // which is like C's char sizes. u8 really.
    llvm::TypeSize rhs_typesize = dl.getTypeAllocSize(rhs_type);


    // returns true if this type is a valid type
    // for a register during codegen.
    if (!(rhs_type->isSingleValueType()))
    {
      llvm::IntegerType* int64_ty = llvm::IntegerType::get(env.GetContext(), /* bitwidth */ 64);
      // emit a memcpy from the temp allocation
      // to the bind allocation.
      llvm::CallInst* call_inst = bldr.CreateMemCpyInline(local_storage,
                                                          local_storage->getAlign(),
                                                          rhs_literal,
                                                          rhs_literal->getPointerAlignment(),
                                                          llvm::ConstantInt::get(int64_ty, rhs_typesize->getFixedSize()));
    }
    else
    {
      llvm::StoreInst* store_inst  = bldr.CreateStore(rhs_value, local_storage);
    }

    env.GetSymbolTable().Bind(name, local_storage);
    // the result of a bind expression is the value that
    // we stored within the location. this allows a composition
    // of bind expressions to allocate and store copies of
    // the value that was built by the rhs.
    // if we were to instead return the local allocation,
    // i don't think it would break memory semantics or
    // anything, but wouldn't the result of storing
    // the allocation instruction locally mean that we were
    // building the other name as an alias of the inner
    // name? so a composition of names would end up being a
    // chain of pointers to eachother till we got to the
    // innermost value?
    return Judgement (std::make_shared<ValueLiteral>(loc, rhs_value));
  }
  else
  {
    // if we don't have an insertion_block
    // then we are binding in the global scope.
    llvm::GlobalVariable global_value(env.GetModule(), /
                                      rhs_type, /
                                      false, /* const? */ /
                                      llvm::GlobalValue::LinkageTypes::InternalLinkage, /
                                      rhs_value, /
                                      std::string(name));
    return Judgement (std::make_shared<ValueLiteral>(loc, rhs_value));
  }
}

}
