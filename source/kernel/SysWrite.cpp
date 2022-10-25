#include "kernel/SysWrite.h"

#include "support/LLVMErrorToString.h"

#include "llvm/IR/InlineAsm.h"

namespace pink {
auto SysWriteSlice(llvm::Value *file_descriptor, llvm::StructType *slice_type,
                   llvm::Value *slice_ptr, const Environment &env)
    -> llvm::Value * {
  llvm::Type *pointer_type = env.instruction_builder->getPtrTy();
  llvm::Type *integer_type = env.instruction_builder->getInt64Ty();
  // before we can emit inline assembly, we first have to load
  // the pointer and compute the write_size from the slice.
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

  auto *buffer_ptr =
      env.instruction_builder->CreateLoad(pointer_type, slice_ptr_ptr);
  return SysWrite(file_descriptor, write_size, buffer_ptr, env);
}

auto SysWriteText(llvm::Value *file_descriptor, llvm::StructType *text_type,
                  llvm::Value *text_ptr, const Environment &env)
    -> llvm::Value * {
  llvm::Type *integer_type = env.instruction_builder->getInt64Ty();

  auto *text_size_ptr = env.instruction_builder->CreateConstInBoundsGEP2_32(
      text_type, text_ptr, 0, 0);
  auto *text_buffer_ptr = env.instruction_builder->CreateConstInBoundsGEP2_32(
      text_type, text_ptr, 0, 1);

  auto *text_size =
      env.instruction_builder->CreateLoad(integer_type, text_size_ptr);

  return SysWrite(file_descriptor, text_size, text_buffer_ptr, env);
}

// construct a call to the sys_write x86-64 linux system call
// to do this we must generate the following x86 assembly:
// mov rdi, $file_descriptor ; "={rdi},(r|i)"
// mov rsi, $buffer          ; "={rsi},r"
// mov rdx, $size            ; "={rdx},r"
// mov rax, 1                ; "={rax},i"
// syscall                   ; "={rax}"
// ; then rax holds the result
auto SysWrite(llvm::Value *file_descriptor, llvm::Value *size,
              llvm::Value *buffer, const Environment &env) -> llvm::Value * {
  assert(file_descriptor->getType()->isIntegerTy(64));
  assert(size->getType()->isIntegerTy(64));
  assert(buffer->getType()->isPointerTy());
  llvm::Type *pointer_type = env.instruction_builder->getPtrTy();
  llvm::Type *integer_type = env.instruction_builder->getInt64Ty();
  llvm::Value *sys_write = env.instruction_builder->getInt64(1);

  // the mov instructions take the integer value to load as their argument.
  // and as a result of the mov, the destination register holds the value.
  // so we can use the loaded register as the 'return type' of 'calling'
  // the inline asm statement.
  auto *mov_rdi_type =
      llvm::FunctionType::get(integer_type, {integer_type}, false);
  auto *mov_rsi_type =
      llvm::FunctionType::get(pointer_type, {pointer_type}, false);
  auto *mov_rdx_type =
      llvm::FunctionType::get(integer_type, {integer_type}, false);
  auto *mov_rax_type =
      llvm::FunctionType::get(integer_type, {integer_type}, false);
  auto *syscall_asm_type =
      llvm::FunctionType::get(integer_type, llvm::None, false);

  assert(syscall_asm_type->getNumParams() == 0);

  llvm::InlineAsm *mov_rdi = nullptr;
  if (auto *constant_fd = llvm::dyn_cast<llvm::ConstantInt>(file_descriptor)) {
    if (auto error = llvm::InlineAsm::verify(mov_rdi_type, "={rdi},i")) {
      std::string errstr =
          "constraint code [={rdi},i] for inline asm [mov rdi, "
          "$file_descriptor] is invalid " +
          LLVMErrorToString(error);
      FatalError(errstr, __FILE__, __LINE__);
    }
    mov_rdi = llvm::InlineAsm::get(
        mov_rdi_type, "mov rdi, $1", "={rdi},i",
        /* hasSideEffects = */ true, /* isAlignStack = */ false,
        llvm::InlineAsm::AD_Intel, /* canThrow = */ false);

  } else { // file_descriptor is an integer loaded from memory
    if (auto error = llvm::InlineAsm::verify(mov_rdi_type, "={rdi},r")) {
      std::string errstr =
          "constraint code [={rdi},r] for inline asm [mov rdi, "
          "$file_descriptor] is invalid " +
          LLVMErrorToString(error);
      FatalError(errstr, __FILE__, __LINE__);
    }
    mov_rdi = llvm::InlineAsm::get(
        mov_rdi_type, "mov rdi, $1", "={rdi},r",
        /* hasSideEffects = */ true, /* isAlignStack = */ false,
        llvm::InlineAsm::AD_Intel, /* canThrow = */ false);
  }

  if (auto error = llvm::InlineAsm::verify(mov_rsi_type, "={rsi},r")) {
    std::string errstr = "constraint code [={rsi},r] for inline asm [mov rsi, "
                         "$buffer] is invalid " +
                         LLVMErrorToString(error);
    FatalError(errstr, __FILE__, __LINE__);
  }
  auto *mov_rsi = llvm::InlineAsm::get(
      mov_rsi_type, "mov rsi, $1", "={rsi},r",
      /* hasSideEffects = */ true, /* isAlignStack = */ false,
      llvm::InlineAsm::AD_Intel, /* canThrow = */ false);

  if (auto error = llvm::InlineAsm::verify(mov_rdx_type, "={rdx},r")) {
    std::string errstr = "constraint code [={rdx},r] for inline asm [mov rdx, "
                         "$size] is invalid " +
                         LLVMErrorToString(error);
    FatalError(errstr, __FILE__, __LINE__);
  }
  auto *mov_rdx = llvm::InlineAsm::get(
      mov_rdx_type, "mov rdx, $1", "={rdx},r",
      /* hasSideEffects = */ true, /* isAlignStack = */ false,
      llvm::InlineAsm::AD_Intel, /* canThrow = */ false);

  if (auto error = llvm::InlineAsm::verify(mov_rax_type, "={rax},i")) {
    std::string errstr = "constraint code [={rax},i] for inline asm [mov rax, "
                         "$1] is invalid " +
                         LLVMErrorToString(error);
    FatalError(errstr, __FILE__, __LINE__);
  }
  auto *mov_rax = llvm::InlineAsm::get(
      mov_rax_type, "mov rax, $1", "={rax},i",
      /* hasSideEffects = */ true, /* isAlignStack = */ false,
      llvm::InlineAsm::AD_Intel, /* canThrow = */ false);

  if (auto error = llvm::InlineAsm::verify(syscall_asm_type, "={rax}")) {
    std::string errstr =
        "constraint code [={rax}] for inline asm [syscall] is invalid " +
        LLVMErrorToString(error);
    FatalError(errstr, __FILE__, __LINE__);
  }
  auto *syscall = llvm::InlineAsm::get(
      syscall_asm_type, "syscall", "={rax}",
      /* hasSideEffects = */ true, /* isAlignStack = */ false,
      llvm::InlineAsm::AD_Intel, /* canThrow = */ false);

  // lets hope the order we emit these don't destroy the results
  // of any instructions emitted earlier!

  // this line is causing an assertion failure. the assertion seems to be
  // checking both the param number and the type of the param.
  // This is logical. But we are also asserting that size has integer_type
  // this assertion is passing.
  // and we declared the function type to pass a single argument, and
  // we are only passing the single argument size, so what is causing the
  // assertion failure? upon further testing it seems that the third
  // inline asm statement we are emitting is causing the assertion
  // failure. no matter which instruction it is. The assertion error
  // moves when I change the constraint codes of the syscall instruction.
  env.instruction_builder->CreateCall(mov_rax_type, mov_rax, {sys_write});
  env.instruction_builder->CreateCall(mov_rdi_type, mov_rdi, {file_descriptor});
  env.instruction_builder->CreateCall(mov_rdx_type, mov_rdx, {size});
  env.instruction_builder->CreateCall(mov_rsi_type, mov_rsi, {buffer});
  return env.instruction_builder->CreateCall(syscall_asm_type, syscall,
                                             llvm::None);
}
} // namespace pink