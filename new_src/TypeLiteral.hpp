#pragma once
#include <string>
#include <memory>

#include "llvm/IR/Type.h"

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Object.hpp"

class TypeLiteral : public Object
{
private:
  llvm::Type* literal;

virtual Judgement GetypeV(const Environment& env) override;
public:
  TypeLiteral(const Location& loc, llvm::Type* literal);
  virtual ~TypeLiteral() = default;

  llvm::Type* GetLiteral();

  virtual std::shared_ptr<TypeLiteral> Clone() override;
  virtual std::string ToString() override;


  virtual Judgement Codegen(const Environment& env) override;
};
