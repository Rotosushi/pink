#pragma once
#include <string>
#include <memory>

#include "llvm/Support/Alignment.h"

#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Ast.hpp"

namespace pink {

class Pointer : public Ast
{
private:
  llvm::Value* pointer;
  llvm::Align  alignment;

  virtual Judgement GetypeV(const Environment& env) override;
public:
  // expects that the value is already a reference
  Pointer(const Location& loc, llvm::Value* pointer, llvm::Align alignment);
  // allows building a nullptr to some type easily.
  Pointer(const Location& loc, llvm::PointerType* pointer_type, llvm::Align alignment);

  virtual std::shared_ptr<Pointer> Clone() override;
  virtual std::string ToString() override;

  llvm::Value* GetPointer();
  llvm::Align  GetAlignment();

  static bool classof(const Ast* ast);

  virtual Judgement Codegen(const Environment& env) override;
};

}
