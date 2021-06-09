#pragma once
#include <string>
#include <memory>

#include "llvm/IR/Value.h"

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Ast.hpp"

class ValueLiteral : public Ast
{
private:
  llvm::Value* literal;

  virtual Judgement GetypeV(const Environment& env) override;
public:
  ValueLiteral(const Location& loc, llvm::Value* literal);
  virtual ~ValueLiteral() = default;

  static bool  classof(const Ast* ast);
  llvm::Value* GetLiteral();

  std::shared_ptr<ValueLiteral> Clone();
  std::string ToString();

  virtual Judgement Codegen(const Environment& env) override;
};
