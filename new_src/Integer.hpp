#pragma once
#include <cstdint>
#include <string>
#include <memory>

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Object.hpp"

class Integer : public Object
{
private:
  int64_t value;

public:
  Integer(const Location& loc, int64_t value);
  virtual ~Integer() = default;

  virtual std::shared_ptr<Integer> Clone() override;
  virtual std::string ToString() override;

  virtual Judgement Getype(const Environment& env) override;
  virtual Judgement Codegen(const Environment& env) override;
};
