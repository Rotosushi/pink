#pragma once
#include <vector>
#include <utility>
#include <string>
#include <memory>

#include "llvm/IR/Value.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Constants.h"

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Argument.hpp"
#include "Ast.hpp"
#include "Pointer.hpp"

/*
  normally an application of a procedure which fails to
  provide all of the values at time of application is considered
  a malformed application of the procedure, because we cannot
  find a value for every argument, and are therefore prevented
  from jumping into the procedure body and evaluating it.

  unless you are working with the lambda calculus, and functional
  languages, in which case you can conceive of returning the partially
  evaluated procedure, because you are generally talking about a
  interpreter doing the real work of evaluation, which means
  you usually have a dynamic notion of a procedure, not a static one.
  and in C, the language's design is reflective of this fact.
  C's notion of a procedure is more in line with the reality of
  what is stored on the computers hardware. what is being talked
  about, in all cases is a sequence of assembly instructions.
  since it is expensive to copy a procedure body around, instead C
  says procedures are not first class values. you can call a procedure,
  but it is not a thing that can be copied/assigned,
  constructed in a local context, or defined at runtime.
  now, i'm absolutely not going to argue that our languages kernel
  should have a notion of defining new procedures at runtime,
  because that presupposes the existance of a full working version
  of the language within the runtime of anything the language
  compiles. something we are explicitly trying to get away from,
  now should the user wish to link in the facilities to do so
  in a program they are writing, that is a different story entierly.

  some programs are well served with an embedded interpreter.


  what would it take to conceive of an object which represented a partially
  applied lambda? why wouldn't it work to store it as a sum, whose size
  is 1, for the function pointer, plus N, for number of actual arguments provided.
  this sum, called PartiallyAppliedLambda supports Getype, and can be stored
  at runtime, allowing more than one to be created. you could fill a vector
  with them for instance. (it's all pointers to the same function, you are
  just buffering the call on-demand.)
  the type of a PartiallyAppliedLambda is the type of the res of the lambda,
  that is, the type of the body of innermost lambda of the curry chain which
  does not have an available binding in the sum. (if you thought of it like a
  tuple, with each actual argument in a slot, with the initial slot filled
  with the function pointer, than each actual argument would be stored in
   tuple[formal-argument-position + 1] )

   where do we construct these objects? well,
  instead of immediately trying and failing to apply a procedure
  when we were not given enough arguments, we choose to instead
  construct a PartiallyAppliedLambda structure, on the stack.
  this holds a by-value copy of each provided argument, just like
  what we do to call the procedure, and it also holds the address of
  the procedure. this object can be passed around just like a
  regular function, in fact this same object, with no bindings is
  equivalent to a runtime allocation of a function.
  we could, when the programmer requests to store a procedure,
  choose to store a PartiallyAppliedLambda object, which supports

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

  fancy-fun := fancy fun;

  which is a callable object with type signature
  T1->T2->T3->T4

  we can then say

  fancy-fun-x := f1 x;

  which has type signature

  (T2->T3->T4)

  we can then say

  fancy-fun-x-y := f2 y;

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

  (we can consider overloading with this definition as well,
   if more than one set of types is bound to the same name, then
   we can disambiguate by relying on the types of the actual arguments
   provided.)

   concept, n-ary tree representing the overload set.
   the root node (singular) presents a list of the first arguments,
   the second level presents each of the second arguments of each the
   bound first arguments, and so forth down the levels of the tree.

   then typing an application of an overloaded procedure becomes,
   see if we can find the first actual argument provided within the
   root node, we can then find a link there to the
   second arguments node, if we have another actual argument present in the
   application tree and the second node expects another argument then we
   can apply it and repeat the above process with the third arguments node,
   if the second argument node doesn't expect another argument then
   we reach another descision, if we have another argument then we are walking
   a malformed tree which is providing more arguments than the function is
   defined to handle. however if we don't have another argument then
   this is a good application, and we can emit an application of the
   link held within this last node, which is a pointer to the function.






*/

/*
  we can represent this as an anonymous structure type
  with a form that looks like:

  {function-pointer, actual-arg0, actual-arg1, ..., actual-argn}

  and whose type is derived from the function-pointer type,
  and the list of actual argument types. this object
  has a type equivalent to the function pointers type,
  except every formal argument that could be bound to
  an actual argument present within the list is considered
  implicit within the type. so the explicit type becomes
  the remaining list of formal arguments, which is always
  smaller than the function pointers type.


  so, it's kind of funny, but when the user types

  (\x:Int=>\y:Int=>x+y) 2 44

  the evaluation of the parenthesized term will
  result in a Partially Applied Lambda, before we
  even get to the Apply term. this way, Apply
  only needs to contend with function objects,
  and things that act like functions only need
  to generate function objects.
  in this way, the code generated to represent
  this term can be represented locally
  within another function, or global variable.
  and other places, with some number of arguments
  bound.


  storage wise this is just a tuple,
  and the more i think about the actual implementation,
  the more i want it to just be a tuple, and then
  Lambda, defines the procedure as a side effect, and
  returns a pointer to a dynamicaly allocated tuple
  with a pointer to the definition, plus a nil value (no closed arguments).
  a function definition does the same, defines the function
  and returns this tuple, the difference is that the tuple
  is what is bound within our symboltable. (and thus,
  what gets written to local storage.) note that with
  the function definition, the tuple is useless except as
  a carrier for the address of the function.
*/

namespace pink {

class Closure : public Ast
{
private:
  Pointer procedure;
  std::vector<std::pair<InternedString, llvm::Constant*>> values;

  virtual Judgement GetypeV(const Environment& env) override;
public:
  Closure(const Location& loc, llvm::Function* fn_ptr); // constructs a closure
                                                        // providing no actual arguments.
  // consdtruct a closure with a list of closed over arguments.
  Closure(const Location& loc, llvm::Function* fn_ptr, std::vector<std::pair<InternedString, llvm::Constant*>>& args);
  virtual ~Closure() = default;

  static bool classof(const Ast* ast);

  llvm::Constant* GetFunction();

  // for getting at the arguments.
  using iterator = std::vector<std::pair<InternedString, llvm::Constant*>>::iterator;

  iterator begin();
  iterator end();

  virtual std::shared_ptr<Closure> Clone() override;
  virtual std::string ToString() override;

  virtual Judgement Codegen(const Environment& env) override;
};

}
