#pragma once
#include <vector>
#include <utility>
#include <string>
#include <memory>

#include "llvm/IR/Value.h"

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Ast.hpp"
#include "Object.hpp"

/*
  normally an application of a procedure which fails to
  provide all of the values at time of application is considered
  a malformed application of the procedure, because we cannot
  find a value for every argument, and are therefore prevented
  from jumping into the procedure body and evaluating it.

  however, what if we instead allowed partial applications?

  instead of immediately trying and failing to apply a procedure
  when we were not given enough arguments, we choose to instead
  construct a PartiallyAppliedLambda structure, on the stack.
  this holds a by-value copy of each provided argument, just like
  what we do to call the procedure, and it also holds the address of
  the procedure.
  a PartiallyAppliedLambda object is a Callable object, whose
  call signature is equivalent to the calling signature of the
  referent procedure, except that each of the provided arguments
  is considered already bound, and is not considered to participate
  within application, except to be bound once we actually want to
  jump into the procedure. we could imagine the procedure

  add := \x:int=>\y:Int => x + y;

  add is a callable object with the
  signature
  (Int -> Int -> Int)

  addone := (add 1)

  addone is a struct, which is a callable object with the
  signature
  (Int -> Int)

  if we had some fancy procedure

  fancy := \f:T1->T2->T3->t4=>\x:T1=>\y:T2=>\z:T3=> f x y z

  fancy's type is
  (T1->T2->T3->T4) -> T1 -> T2 -> T3 -> T4

  x := T1(...);
  y := T2(...);
  z := T3(...);

  if we presume the existance of a procedure (fun)
  with the appropriate type signature (T1->T2->T3->T4),
  then we can say:

  f1 := fancy fun;

  which is a callable object with type signature
  T1->T2->T3->T4

  we can then say

  f2 := f1 x;

  which has type signature

  (T2->T3->T4)

  we can then say

  f3 := f2 y;

  which has type signature

  (T3 -> T4)

  and we can finally say

  result := f3 z;

  which since we now have the proper number of
  arguments we can fully apply the procedure.

  result then has the type signature

  (T4)

  so, the result of applying a PartiallyAppliedLambda is
  either another PartiallyAppliedLambda with one or more
  arguments bound, and one or more arguments left to bind.
  or it results in a full application of the underlying procedure
  providing all of the bindings that have been collected up to
  this point.
  however, the choice of which is deterministic with respect to
  the number of actual arguments present in the call expression,
  the number of arguments bound already within the partially applied lambda,
  and the number of arguments the underlying procedure needs.

*/
class PartiallyAppliedLambda : public Object
{
private:
  std::vector<std::pair<InternedString, std::shared_ptr<Ast>>> args;
  InternedString lambda;

public:
  PartiallyAppliedLambda(const Location& loc, std::vector<std::pair<Interned_string, std::shared_ptr<Ast>>> args, InternedString lambda);
  virtual ~PartiallyAppliedLambda() = default;

  virtual std::shared_ptr<PartiallyAppliedLambda> Clone() override;
  virtual std::string ToString() override;

  virtual Judgement Getype(const Environment& env) override;
  virtual Judgement Codegen(const Environment& env) override;
};
