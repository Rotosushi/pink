#pragma once
#include <string>
#include <memory>

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Object.hpp"

class Boolean : public Object
{
private:
  bool value;

public:
  Boolean(const Location& loc, bool val);
  virtual ~Boolean() = default;

  virtual std::shared_ptr<Boolean> Clone() override;
  virtual std::string ToString() override;

  virtual Judgement Getype(const Environment& env) override;
  virtual Judgement Codegen(const Environment& env) override;
};
