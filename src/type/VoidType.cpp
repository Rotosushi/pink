#include "type/VoidType.h"

#include "aux/Environment.h"

namespace pink 
{
  VoidType::VoidType()
    : Type(Type::Kind::Void)
  {

  }

  VoidType::~VoidType()
  {

  }

  bool VoidType::classof(const Type* t)
  {
    return t->getKind() == Type::Kind::Void;
  }

  bool VoidType::EqualTo(Type* other)
  {
    return other->getKind() == Type::Kind::Void;
  }

  std::string VoidType::ToString()
  {
    return std::string("Void");
  }

  Outcome<llvm::Type*, Error> VoidType::Codegen(std::shared_ptr<Environment> env)
  {
    return env->builder->getVoidTy();
  }
}
