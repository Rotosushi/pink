#include "type/TupleType.h"

#include "aux/Environment.h"

#include "llvm/IR/DerivedTypes.h"

namespace pink {
TupleType::TupleType(const std::vector<Type *> &member_types)
    : Type(Type::Kind::Tuple), member_types(member_types) {}

auto TupleType::classof(const Type *type) -> bool {
  return type->GetKind() == Type::Kind::Tuple;
}

auto TupleType::EqualTo(Type *other) const -> bool {
  bool result = true;
  auto *tuple_type = llvm::dyn_cast<TupleType>(other);
  if (tuple_type != nullptr) {
    if (member_types.size() == tuple_type->member_types.size()) {
      for (size_t i = 0; i < member_types.size(); i++) {
        if (member_types[i] != tuple_type->member_types[i]) {
          result = false;
          break;
        }
      }
    } else {
      result = false;
    }
  } else {
    result = false;
  }

  return result;
}

auto TupleType::ToString() const -> std::string {
  std::string result = "(";
  for (size_t i = 0; i < member_types.size(); i++) {
    result += member_types[i]->ToString();
    if (i < (member_types.size() - 1)) {
      result += ", ";
    }
  }
  result += ")";
  return result;
}

auto TupleType::Codegen(const Environment &env) const
    -> Outcome<llvm::Type *, Error> {
  std::vector<llvm::Type *> member_llvm_types(member_types.size());

  for (Type *member_type : member_types) {
    Outcome<llvm::Type *, Error> member_type_codegen_result =
        member_type->Codegen(env);

    if (!member_type_codegen_result) {
      return member_type_codegen_result;
    }

    member_llvm_types.push_back(member_type_codegen_result.GetFirst());
  }

  return llvm::StructType::get(*env.context, member_llvm_types);
}
} // namespace pink
