#pragma once
#include <string>
#include <memory>

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Ast.hpp"


class Variable : public Ast
{
private:
  InternedString name;
  ScopeSet       scope;

public:
  Variable(const Location& loc, InternedString name, ScopeSet scope);
  virtual ~Variable() = default;

  virtual std::shared_ptr<Variable> Clone() override;

  virtual std::string ToString();

  virtual Judgement Getype(const Environment& env) override;
  // using a Judgement as the return type here implies using
  // a std::shared_ptr<Ast> to hold the generated code.
  // given that we use Objects to represent values in pink,
  // this implies giving Object a representation which deals
  // in llvm::Value *'s. this was already going to be the case
  // given we are using llvm to generate the code. we need to
  // represent our values with llvm values to communicate.
  // we still want the success/failure capture with the same
  // type though, to make the mutual recursivce nature of this
  // algorithm cleaner to express in the code.
  virtual Judgement Codegen(const Environement& env) override;
};
