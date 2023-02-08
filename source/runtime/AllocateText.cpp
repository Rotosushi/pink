#include <algorithm>
#include <vector>

#include "runtime/AllocateText.h"
#include "runtime/AllocateVariable.h"

namespace pink {

static auto ToConstantVector(const std::string &text, Environment &env)
    -> std::vector<llvm::Constant *> {
  std::vector<llvm::Constant *> constant_characters(text.size() + 1);
  auto to_constant = [&env](char character) -> llvm::Constant * {
    return env.instruction_builder->getInt8(uint8_t(character));
  };
  std::transform(text.begin(),
                 text.end(),
                 constant_characters.begin(),
                 to_constant);
  // null terminate the string
  constant_characters.back() = env.instruction_builder->getInt8(0);
  return constant_characters;
}

auto AllocateGlobalText(const std::string &name,
                        const std::string &text,
                        Environment       &env) -> llvm::GlobalVariable * {
  auto *bounds_type    = env.instruction_builder->getInt64Ty();
  auto *size           = env.instruction_builder->getInt64(text.size());
  auto *character_type = env.instruction_builder->getInt8Ty();
  auto *array_type{llvm::ArrayType::get(character_type, text.size() + 1)};
  auto  constant_characters{ToConstantVector(text, env)};
  auto *llvm_text{llvm::ConstantArray::get(array_type, constant_characters)};

  auto *text_type{
      llvm::StructType::get(*env.context, {bounds_type, array_type})};
  auto *text_initializer{
      llvm::ConstantStruct::get(text_type, {size, llvm_text})};

  auto *global_text{AllocateGlobal(name, text_type, env)};
  global_text->setInitializer(text_initializer);
  return global_text;
}

} // namespace pink