#pragma once

#include <string>
#include <memory>

#include "Location.hpp"
#include "StringInterner.hpp"
#include "Ast.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"

/*
  the act of naming what was once simply
  a temporary value. in normal calculations
  many intermediate temporary values are needed.
  (essentaially, pulling the specific peices/words of
   data the computer needs to do the work specified by
   the instruction/statement.)
  however, a named value is different, in that it, in the
  most general sense, lives in the stack frame of the
  current procedure. (or in the global data of the program,
  but we will elide that discussion for the moment)
  since we are considering allocating a new variable
  within the current function being defined, to properly
  write the codegen procedure for bind, we need to
  disambiguate between types during the runtime of
  the program. (we will only be given storable types
  by Codegen itself though. right?)
*/

namespace pink {

class Bind : public Ast
{
private:
  InternedString       name;
  std::shared_ptr<Ast> right;

  virtual Judgement GetypeV(const Environment& env) override;
public:
  Bind(const Location& loc, InternedString name, std::shared_ptr<Ast> rhs);
  virtual ~Bind() = default;

  static bool classof(const Ast* ast);

  virtual std::shared_ptr<Bind> Clone() override;
  virtual std::string ToString() override;

  virtual Judgement Codegen(const Environment& env) override;
};

}









--
