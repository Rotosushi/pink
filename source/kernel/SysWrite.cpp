#include "kernel/SysWrite.h"

#include "support/LLVMErrorToString.h"

#include "llvm/IR/InlineAsm.h"

namespace pink {
auto SysWrite(llvm::Value *file_descriptor, llvm::Value *slice_ptr,
              const Environment &env) -> llvm::Value * {
  // construct a call to the sys_write x86-64 linux system call
  // we want to generate the following x86 assembly:
  // mov rdi, $file_descriptor ; "={rdi},(r|i)"
  // mov rsi, $slice_ptr.ptr   ; "={rsi},r"
  // mov rdx, $slice_ptr.size  ; "={rdx},r"
  // syscall                   ; "={rax}"
  // ; then rax holds the result
  llvm::Type *pointer_type = env.instruction_builder->getPtrTy();
  llvm::Type *integer_type = env.instruction_builder->getInt64Ty();
  // before we can emit assembly, we first have to load
  // the pointer and the size from the slice.
  auto *slice_type = slice_ptr->getType();
  auto *slice_size_ptr = env.instruction_builder->CreateConstInBoundsGEP2_32(
      slice_type, slice_ptr, 0, 0);
  auto *slice_offset_ptr = env.instruction_builder->CreateConstInBoundsGEP2_32(
      slice_type, slice_ptr, 0, 1);
  auto *slice_ptr_ptr = env.instruction_builder->CreateConstInBoundsGEP2_32(
      slice_type, slice_ptr, 0, 2);

  auto *size =
      env.instruction_builder->CreateLoad(integer_type, slice_size_ptr);
  auto *offset =
      env.instruction_builder->CreateLoad(integer_type, slice_offset_ptr);

  auto *write_size = env.instruction_builder->CreateSub(size, offset);

  auto *mov_asm_type =
      llvm::FunctionType::get(integer_type, {integer_type}, false);
  auto *syscall_asm_type = llvm::FunctionType::get(integer_type, {}, false);

  llvm::InlineAsm *mov_rdi = nullptr;
  if (auto *constant_fd = llvm::dyn_cast<llvm::ConstantInt>(file_descriptor)) {
    if (auto error = llvm::InlineAsm::verify(mov_asm_type, "={rdi},i")) {
      std::string errstr =
          "constraint code [={rdi},i] for inline asm [mov rdi, "
          "$file_descriptor]" +
          LLVMErrorToString(error);
      FatalError(errstr.data(), __FILE__, __LINE__);
    }

    mov_rdi = llvm::InlineAsm::get(
        mov_asm_type, "mov rdi, $1", "={rdi},i",
        /* hasSideEffects = */ false, /* isAlignStack = */ false,
        llvm::InlineAsm::AD_Intel, /* canThrow = */ false);

  } else { // file_descriptor is an integer pointer to memory (global or local)
    assert((llvm::dyn_cast<llvm::GlobalVariable>(file_descriptor) != nullptr) ||
           (llvm::dyn_cast<llvm::AllocaInst>(file_descriptor) != nullptr) ||
           (file_descriptor->getType()->isPointerTy()));
    if (auto error = llvm::InlineAsm::verify(mov_asm_type, "={rdi},r")) {
      std::string errstr =
          "constraint code [={rdi},r] for inline asm [mov rdi, "
          "$file_descriptor]" +
          LLVMErrorToString(error);
      FatalError(errstr.data(), __FILE__, __LINE__);
    }

    mov_rdi = llvm::InlineAsm::get(
        mov_asm_type, "mov rdi, $1", "={rdi},r",
        /* hasSideEffects = */ false, /* isAlignStack = */ false,
        llvm::InlineAsm::AD_Intel, /* canThrow = */ false);
  }

  if (auto error = llvm::InlineAsm::verify(mov_asm_type, "={rsi},r")) {
    std::string errstr = "constraint code [={rsi},r] for inline asm [mov rsi, "
                         "$slice_ptr.ptr] is invalid" +
                         LLVMErrorToString(error);
    FatalError(errstr.data(), __FILE__, __LINE__);
  }
  auto *mov_rsi = llvm::InlineAsm::get(
      mov_asm_type, "mov rsi, $1", "={rsi},r",
      /* hasSideEffects = */ false, /* isAlignStack = */ false,
      llvm::InlineAsm::AD_Intel, /* canThrow = */ false);

  if (auto error = llvm::InlineAsm::verify(mov_asm_type, "={rdx},r")) {
    std::string errstr = "constraint code [={rdx},r] for inline asm [mov rdx, "
                         "$slice_ptr.size] is invalid" +
                         LLVMErrorToString(error);
    FatalError(errstr.data(), __FILE__, __LINE__);
  }

  auto *mov_rdx = llvm::InlineAsm::get(
      mov_asm_type, "mov rdx, $1", "={rdx},r",
      /* hasSideEffects = */ false, /* isAlignStack = */ false,
      llvm::InlineAsm::AD_Intel, /* canThrow = */ false);

  if (auto error = llvm::InlineAsm::verify(syscall_asm_type, "={rax}")) {
    std::string errstr =
        "constraint code [={rax}] for inline asm [syscall] is invalid" +
        LLVMErrorToString(error);
    FatalError(errstr.data(), __FILE__, __LINE__);
  }

  auto *syscall = llvm::InlineAsm::get(
      syscall_asm_type, "syscall", "={rax}",
      /* hasSideEffects = */ false, /* isAlignStack = */ false,
      llvm::InlineAsm::AD_Intel, /* canThrow = */ false);

  // lets hope the order we emit these don't destroy the results
  // of the load instructions earlier!
  env.instruction_builder->CreateCall(mov_asm_type, mov_rdi, {file_descriptor});

  auto *pointer =
      env.instruction_builder->CreateLoad(pointer_type, slice_ptr_ptr);
  env.instruction_builder->CreateCall(mov_asm_type, mov_rsi, {pointer});

  env.instruction_builder->CreateCall(mov_asm_type, mov_rdx, {write_size});

  return env.instruction_builder->CreateCall(syscall_asm_type, syscall);
}
} // namespace pink