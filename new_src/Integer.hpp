#pragma once
#include <cstdint>
#include <string>
#include <memory>

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Ast.hpp"

/*
  represents an instance of a literal integer within our grammar.
*/

namespace pink {

class Integer : public Ast
{
private:
  int64_t value;

  virtual Judgement GetypeV(const Environment& env) override;
public:
  Integer(const Location& loc); // default value is zero
  Integer(const Location& loc, int64_t value);
  virtual ~Integer() = default;

  static bool classof(const Ast* ast);

  virtual std::shared_ptr<Integer> Clone() override;
  virtual std::string ToString() override;


  virtual Judgement Codegen(const Environment& env) override;
};

}
