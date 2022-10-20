#include <algorithm>
#include <vector>

#include "kernel/AllocateString.h"
#include "kernel/AllocateVariable.h"

namespace pink {

auto AllocateGlobalString(const std::string &name, const std::string &text,
                          const Environment &env) -> llvm::GlobalVariable * {
  auto *bounds_type = env.instruction_builder->getInt64Ty();
  auto *size = env.instruction_builder->getInt64(text.size());
  auto *character_type = env.instruction_builder->getInt8Ty();
  auto *array_type = llvm::ArrayType::get(character_type, text.size() + 1);
  // we store string literals as arrays of characters (i8's),
  // that is the size, plus the array of characters.
  std::vector<llvm::Constant *> string_constant(text.size() + 1);
  auto to_constant = [&env](const char character) -> llvm::Constant * {
    return env.instruction_builder->getInt8(character);
  };
  std::transform(text.begin(), text.end(), string_constant.begin(),
                 to_constant);

  // null terminate the string
  string_constant.back() = env.instruction_builder->getInt8(0);
  auto *string_initializer =
      llvm::ConstantArray::get(array_type, string_constant);

  auto *string_type =
      llvm::StructType::get(*env.context, {bounds_type, array_type});
  auto *string =
      llvm::ConstantStruct::get(string_type, {size, string_initializer});

  auto *string_ptr = AllocateGlobal(name, string_type, env);
  string_ptr->setInitializer(string);
  return string_ptr;
}

} // namespace pink