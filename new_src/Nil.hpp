#pragma once
#include <string>
#include <memory>

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Ast.hpp"

/*
  represents a literal Nil within our grammar.
*/
class Nil : public Ast
{
private:
  virtual Judgement GetypeV(const Environment& env) override;
public:
  Nil(const Location& loc);
  virtual ~Nil() = default;

  static bool classof(const Ast* ast);

  virtual std::shared_ptr<Nil> Clone() override;
  virtual std::string ToString() override;

  virtual Judgement Codegen(const Environment& env) override;
}
