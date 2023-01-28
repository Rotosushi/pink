#include "kernel/AllocateVariable.h"
#include "kernel/StoreValue.h"

#include "support/LLVMValueToString.h"

namespace pink {
auto AllocateVariable(const std::string &name, llvm::Type *type,
                      const Environment &env, llvm::Value *initializer)
    -> llvm::Value * {
  assert(type != nullptr);
  if (env.current_function == nullptr) {
    return AllocateGlobal(name, type, env, initializer);
  }
  return AllocateLocal(name, type, env, initializer);
}

auto AllocateGlobal(const std::string &name, llvm::Type *type,
                    const Environment &env, llvm::Value *initializer)
    -> llvm::GlobalVariable * {
  auto *variable = llvm::cast<llvm::GlobalVariable>(
      env.llvm_module->getOrInsertGlobal(name, type));
  // while not technically a problem, it is bad practice
  // to construct uninitialized global variables. (or global
  // variables at all frankly.)
  if (initializer != nullptr) {
    auto *const_init = llvm::dyn_cast<llvm::Constant>(initializer);
    if (const_init == nullptr) {
      std::string errmsg = "Global variables must have constant initializers, "
                           "initializer was: " +
                           LLVMValueToString(initializer);
      FatalError(errmsg, __FILE__, __LINE__);
    }
    variable->setInitializer(const_init);
  }
  return variable;
}

auto AllocateLocal(const std::string &name, llvm::Type *type,
                   const Environment &env, llvm::Value *initializer)
    -> llvm::AllocaInst * {
  assert(env.current_function != nullptr);
  auto *insertion_block = &(env.current_function->getEntryBlock());
  auto insertion_point = insertion_block->getFirstInsertionPt();
  llvm::IRBuilder local_builder(insertion_block, insertion_point);

  auto *variable = local_builder.CreateAlloca(
      type, env.data_layout.getAllocaAddrSpace(), nullptr, name);

  if (initializer != nullptr) {
    StoreValue(type, variable, initializer, env);
  }
  return variable;
}
} // namespace pink