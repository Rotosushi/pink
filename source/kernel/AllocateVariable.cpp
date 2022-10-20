#include "kernel/AllocateVariable.h"

namespace pink {
auto AllocateGlobal(const std::string &name, llvm::Type *type,
                    const Environment &env) -> llvm::GlobalVariable * {
  return llvm::cast<llvm::GlobalVariable>(
      env.llvm_module->getOrInsertGlobal(name, type));
}

auto AllocateLocal(const std::string &name, llvm::Type *type,
                   const Environment &env) -> llvm::AllocaInst * {
  auto *insertion_block = &(env.current_function->getEntryBlock());
  auto insertion_point = insertion_block->getFirstInsertionPt();
  llvm::IRBuilder local_builder(insertion_block, insertion_point);

  return local_builder.CreateAlloca(type, env.data_layout.getAllocaAddrSpace(),
                                    nullptr, name);
}
} // namespace pink