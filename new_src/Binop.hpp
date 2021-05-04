#pragma once
#include <string>
#include <memory>

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Ast.hpp"


class Binop : public Ast
{
private:
  InternedString op;
  std::shared_ptr<Ast> left;
  std::shared_ptr<Ast> right;

public:
  Binop(const Location& loc, InternedString op, std::shared_ptr<Ast> lhs, std::shared_ptr<Ast> rhs);
  virtual ~Binop() = default;

  virtual std::shared_ptr<Binop> Clone() override;
  virtual std::string ToString() override;

  virtual Judgement Getype(const Environment& env) override;
  virtual Judgement Codegen(const Environment& env) override;
};
