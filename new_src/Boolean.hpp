#pragma once
#include <string>
#include <memory>

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Ast.hpp"

namespace pink {
/*
  represents a literal boolean within
  our grammar.
*/
class Boolean : public Ast
{
private:
  bool value;

  virtual Judgement GetypeV(const Environment& env) override;
public:
  Boolean(const Location& loc);
  Boolean(const Location& loc, bool val);
  virtual ~Boolean() = default;

  static bool classof(const Ast* ast);

  virtual std::shared_ptr<Boolean> Clone() override;
  virtual std::string ToString() override;


  virtual Judgement Codegen(const Environment& env) override;
};

}
