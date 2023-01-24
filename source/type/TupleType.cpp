#include "type/TupleType.h"

#include "aux/Environment.h"

#include "llvm/IR/DerivedTypes.h"

namespace pink {
/*
auto TupleType::EqualTo(Type *other) const -> bool {
  bool result = true;
  auto *tuple_type = llvm::dyn_cast<TupleType>(other);
  if (tuple_type != nullptr) {
    if (elements.size() == tuple_type->elements.size()) {
      for (size_t i = 0; i < elements.size(); i++) {
        if (elements[i] != tuple_type->elements[i]) {
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
  for (size_t i = 0; i < elements.size(); i++) {
    result += elements[i]->ToString();
    if (i < (elements.size() - 1)) {
      result += ", ";
    }
  }
  result += ")";
  return result;
}

auto TupleType::ToLLVM(const Environment &env) const -> llvm::Type * {
  std::vector<llvm::Type *> llvm_member_types;

  auto transform_member = [&env](Type *type) { return type->ToLLVM(env); };

  std::transform(elements.begin(), elements.end(), llvm_member_types.begin(),
                 transform_member);

  return llvm::StructType::get(*env.context, llvm_member_types);
}
*/
} // namespace pink
