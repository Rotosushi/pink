#pragma once

#include "type/Type.h"

namespace pink {
  class VoidType : public Type {
  public:
    VoidType();
    virtual ~VoidType();

    static bool classof(const Type* t);
    
    virtual bool EqualTo(Type* other) override;
    virtual std::string ToString() override;

    virtual Outcome<llvm::Type*, Error> Codegen(std::shared_ptr<Environment> env) override;
  };
}
