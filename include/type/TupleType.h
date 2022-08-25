#pragma once 
#include <vector>

#include "type/Type.h"

namespace pink {
  class TupleType : public Type {
  public:
    std::vector<Type*> member_types;

    TupleType(std::vector<Type*> member_types);
    virtual ~TupleType();

    static bool classof(const Type* type);

    virtual bool EqualTo(Type* other) override;
    virtual std::string ToString() override;

    virtual Outcome<llvm::Type*, Error> Codegen(std::shared_ptr<Environment> env) override;
  };
}
