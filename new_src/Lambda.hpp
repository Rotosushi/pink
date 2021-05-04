#pragma once
#include <vector>
#include <utility>
#include <string>
#include <memory>

#include "llvm/IR/Type.h"

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Object.hpp"

class Lambda : public Object
{
private:
  std::vector<std::pair<InternedString, llvm::Type*>> args;
  std::unique_ptr<SymbolTable> symbols;
  std::shared_ptr<Ast> body;

public:
  Lambda(const Location& loc, std::vector<std::pair<InternedString, llvm::Type*>> args, SymbolTable* outer_symbols, std::shared_ptr<Ast> body);
  virtual ~Lambda() = default;

  virtual std::shared_ptr<Lambda> Clone() override;
  virtual std::string ToString() override;

  virtual Judgement Getype(const Environment& env) override;
  virtual Judgement Codegen(const Environment& env) override;
};
