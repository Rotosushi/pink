#pragma once
#include <string>
#include <memory>

#include "llvm/IR/Type.h"

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Ast.hpp"

namespace pink {

class TypeLiteral : public Ast
{
private:
  llvm::Type* literal;

virtual Judgement GetypeV(const Environment& env) override;
public:
  TypeLiteral(const Location& loc, llvm::Type* literal);
  virtual ~TypeLiteral() = default;

  static bool classof(const Ast* ast);
  llvm::Type* GetLiteral();

  virtual std::shared_ptr<TypeLiteral> Clone() override;
  virtual std::string ToString() override;


  virtual Judgement Codegen(const Environment& env) override;
};

}
