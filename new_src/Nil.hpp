#pragma once
#include <string>
#include <memory>

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Object.hpp"

class Nil : public Object
{
public:
  Nil(const Location& loc);
  virtual ~Nil() = default;

  virtual std::shared_ptr<Nil> Clone() override;
  virtual std::string ToString() override;

  virtual Judgement Getype(const Environment& env) override;
  virtual Judgement Codegen(const Environment& env) override;
}
