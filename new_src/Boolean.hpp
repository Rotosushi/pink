#pragma once
#include <string>
#include <memory>

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Object.hpp"

/*
  represents a literal boolean within
  our grammar.
*/
class Boolean : public Object
{
private:
  bool value;

  virtual Judgement GetypeV(const Environment& env) override;
public:
  Boolean(const Location& loc, bool val);
  virtual ~Boolean() = default;

  virtual std::shared_ptr<Boolean> Clone() override;
  virtual std::string ToString() override;


  virtual Judgement Codegen(const Environment& env) override;
};
