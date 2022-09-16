#pragma once 
#include "type/Type.h"

namespace pink {
  class ArrayType : public Type {
  public:
    size_t size;
    Type*  member_type;

    ArrayType(size_t size, Type* member_type);
    ~ArrayType();

    static bool classof(const Type* type);

    virtual bool EqualTo(Type* other) override;
    virtual std::string ToString() override;

    virtual Outcome<llvm::Type*, Error> Codegen(const Environment& env) override;
  };
}

