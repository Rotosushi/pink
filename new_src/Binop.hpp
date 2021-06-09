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

  virtual Judgement GetypeV(const Environment& env) override;
public:
  Binop(const Location& loc, InternedString op, std::shared_ptr<Ast> lhs, std::shared_ptr<Ast> rhs);
  virtual ~Binop() = default;

  static bool classof(const Ast* ast);

  virtual std::shared_ptr<Binop> Clone() override;
  virtual std::string ToString() override;


  virtual Judgement Codegen(const Environment& env) override;
};
