#pragma once
#include <string>
#include <memory>

#include "llvm/IR/Value.h"

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Object.hpp"

class ValueLiteral : public Object
{
private:
  llvm::Value* literal;

public:
  ValueLiteral(const Location& loc, llvm::Value* literal);
  virtual ~ValueLiteral() = default;

  virtual std::shared_ptr<ValueLiteral> Clone() override;
  virtual std::string ToString() override;

  virtual Judgement Getype(const Environment& env) override;
  virtual Judgement Codegen(const Environment& env) override;
};
