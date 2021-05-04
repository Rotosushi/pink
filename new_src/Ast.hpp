#pragma once
#include <string>
#include <memory>

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"

class Ast
{
protected:
  Location loc;

public:
  Ast(const Location& loc);
  virtual ~Ast() = default;

  Location GetLocation();

  virtual std::shared_ptr<Ast> Clone() = 0;
  virtual std::string ToString() = 0;

  // Getype does static analysis, and returns a Judgement,
  // a Judgement is a simple box that encapsulates the return
  // value for the success route and the failure route within
  // the same return type. in this way, the logic of each
  // procedure only needs to check success, and can simply
  // early return failure. which keeps the procedure logic simpler.
  // now that we are using LLVM however, and we are going to leverage
  // their JIT to do Interpretation. this means that we are
  // not going to implement in tree evaluation like we have been,
  // and it also means that we might need a LLVM::Value literal Object.
  // so we can smuggle the code that is being generated out within a
  // Judgement. Judgement holds a std::shared_ptr<Ast> as it were.
  // note that this is essentially the same strategy we used with
  // Type Literals. we consider an Ast Node which represents a TypeLiteral.
  // and then it could hold our Interned Type Ptr. this is how Getype
  // returns a successful type from static analysis.
  // so why not have Codegen do the same with a llvm::Value* ?
  // to me, it just seems a bit weird, and there isn't the
  // internededness of Value*'s. However, we do want to compose
  // with the result types of other Codegen proceedures,
  // (in fact this is exactly where the impetous behind the
  //  Judgement class itself comes from. we either want to compose
  // successes together, or report the failure, both are known
  // and valid paths.) so, from that perspective we are solving
  // the same problem. and from that same perspective we are going
  // to add a LLVMValueLiteral Object.
  virtual Judgement Getype(const Environment& env) = 0;
  virtual Judgement Codegen(const Environment& env) = 0;
};
