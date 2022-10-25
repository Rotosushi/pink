#include <algorithm>
#include <vector>

#include "kernel/AllocateText.h"
#include "kernel/AllocateVariable.h"

namespace pink {

auto AllocateGlobalText(const std::string &name, const std::string &text,
                        const Environment &env) -> llvm::GlobalVariable * {
  auto *bounds_type = env.instruction_builder->getInt64Ty();
  auto *size = env.instruction_builder->getInt64(text.size());
  auto *character_type = env.instruction_builder->getInt8Ty();
  auto *array_type = llvm::ArrayType::get(character_type, text.size() + 1);

  std::vector<llvm::Constant *> constant_characters(text.size() + 1);
  auto to_constant = [&env](char character) -> llvm::Constant * {
    return env.instruction_builder->getInt8(uint8_t(character));
  };
  std::transform(text.begin(), text.end(), constant_characters.begin(),
                 to_constant);
  // null terminate the string
  constant_characters.back() = env.instruction_builder->getInt8(0);
  auto *llvm_text = llvm::ConstantArray::get(array_type, constant_characters);

  // #RULE we store string literals (text) as arrays of characters (i8's)
  auto *text_type =
      llvm::StructType::get(*env.context, {bounds_type, array_type});
  auto *text_initializer =
      llvm::ConstantStruct::get(text_type, {size, llvm_text});

  auto *global_text = AllocateGlobal(name, text_type, env);
  global_text->setInitializer(text_initializer);
  return global_text;
}

} // namespace pink