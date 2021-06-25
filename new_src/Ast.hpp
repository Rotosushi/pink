#pragma once
#include <string>
#include <memory>

// https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html
#include "llvm/Support/Casting.h"

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"

namespace pink {

class Ast
{
  // our defined metadata, we want access to these values
  // within our baseclasses.
protected:
  Location loc;
  llvm::Type* cached_type;

  // we hide the virtual call so we can tuck away the cached
  // type in the concrete call.
  virtual Judgement GetypeV(const Environment& env) = 0;

// https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html
// this is for LLVM style RTTI, which is based
// on enumerations, just like dynamic typeinformation
// in C.
// so, from reading the documentation, the only classes that actually
// need to participate in the is-a analysis are the ones that
// can actually be instanciated. this has the effect of us not
// needing to specify abstract classes in this enumeration, meaning
// we do not specify the abstract base class.
// I am reflecting on that to say we also directly specify our
// objects themselves, and not the virtual Object class.
// despite the fact that Object is a class in our heirarchy.
// so maybe object is truly useless if we are not defining our own
// evaluator.
public:
  // this spot circularly/recursively requires knowledge of all
  // the classes in the heirarchy a-priori to definition.
  // this is the flag that tells llvm which class is active
  // when we see an Ast*, this is used via 'classof' by llvm
  // to implement llvm::cast<>(), llvm::isa<>(), llvm::dyn_cast<>()
  enum Kind
  {
  // Language Terms for Typing and Code generation
    Empty,
    TypeLiteral,
    ValueLiteral,
  // Symbolic Computation
    Variable,
    Bind,
    Application,
    Assignment,
    // Conditional,
    // Iteration,
    // RunFirstIteration,
    Binop,
    Unop,
  // Objects
    Nil,
    Boolean,
    Integer,
    Pointer,
    // Unsigned
    // i8/16/32/64, s8/16/32/64
    // Char
    // Float
    // Reference/Pointer
    // Enumeration
    // Array
    // String
    // Tuple
    // Union
    // type definitions/aliasing
  // Abstraction (which is-a Object)
    Lambda,
    Closure,
    // Function,
    // Coroutine
  };

private:
  const Kind kind;

// the rest of our class
public:

  Ast(const Kind kind, const Location& loc);
  virtual ~Ast() = default;

  AstKind  GetKind() const;
  Location GetLocation();

  virtual std::shared_ptr<Ast> Clone() = 0;
  virtual std::string ToString() = 0;

  // Getype does static analysis, and returns a Judgement,
  // a Judgement is a simple box that encapsulates the return
  // value for the success route and the failure route within
  // the same return type. in this way, the logic of each
  // procedure only needs to check success, and can simply
  // early return failure. which keeps the procedure logic simpler.
  Judgement Getype(const Environment& env);
  virtual Judgement Codegen(const Environment& env) = 0;
};

}
