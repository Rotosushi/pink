#pragma once
#include <string>
#include <memory>

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Ast.hpp"

namespace pink {

class Unop : public Ast
{
private:
  InternedString op;
  std::shared_ptr<Ast> right;

  virtual Judgement GetypeV(const Environment& env);
public:
  Unop(const Location& loc, InternedString op, std::shared_ptr<Ast> rhs);
  virtual ~Unop() = default;

  static bool classof(const Ast* ast);

  virtual std::shared_ptr<Unop> Clone() override;
  virtual std::string ToString() override;


  virtual Judgement Codegen(const Environment& env);
};

}