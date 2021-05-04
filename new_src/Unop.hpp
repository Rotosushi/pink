#pragma once
#include <string>
#include <memory>

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Ast.hpp"

class Unop : public Ast
{
private:
  InternedString op;
  std::shared_ptr<Ast> right;

public:
  Unop(const Location& loc, InternedString op, std::shared_ptr<Ast> rhs);
  virtual ~Unop() = default;

  virtual std::shared_ptr<Unop> Clone() override;
  virtual std::string ToString() override;

  virtual Judgement Getype(const Environment& env);
  virtual Judgement Codegen(const Environment& env);
};
