#include "kernel/sys/SysExit.h"
#include "kernel/Cast.h"

#include "support/LLVMErrorToString.h"

#include "llvm/IR/InlineAsm.h"

namespace pink {
// #NOTE: this is very non-portable, and will need to be made
// separately for each release platform and OS choice.
// #PORTABILITY
// #TODO: refactor this section into a function call which then
// dispatches based upon which target language and OS we are emitting
// for. As of this early version this is an acceptable solution, as
// our language only supports x86-64 on Linux.

// however, in general, inline asm works in llvm by constructing
// the llvm::InlineAsm node with the correct asm string and constraints
// string (specified:
// llvm.org/docs/LangRef.html#inline-assembler-expressions)
// and each asm string must then be used as the argument to
// a call instruction, this call instruction is how we direct
// local variables into the assembly itself. so for this particular
// case, we want to pass it the return value of the body of the
// function.
//
// then, the actual assembly we want to emit is:
//  mov rax, 60
//  mov rdi, $1
//  syscall
//
// because we are only ever using the asm within a 'call' instruction,
// each instruction, in and of itself, has a 'function type' that is,
// a type which describes the arguments to and resulting value type of
// the instruction itself.
//
//  mov rax, 60
// takes no arguments (void), because the value is immediate, and returns an
// integer type.
//
//  mov rdi, $1
// takes a single integer as argument, via $1, which indexes the
// argument list itself, and takes the first available argument.
// it returns an integer just like the above instruction.
//
//  syscall
// takes no arguments (void), and returns it's result in rax.
// in this particular case, we are calling exit(), which does
// not return into our program.
//
void SysExit(llvm::Value *exit_code, const Environment &env) {
  assert(exit_code != nullptr);

  llvm::Type *int64Ty = env.instruction_builder->getInt64Ty();
  llvm::Type *voidTy  = env.instruction_builder->getVoidTy();

  std::vector<llvm::Type *> noArgsTy = {};

  auto *mov_rax_type = llvm::FunctionType::get(int64Ty, noArgsTy, false);
  auto *mov_rdi_type = llvm::FunctionType::get(int64Ty, {int64Ty}, false);
  auto *syscall_type = llvm::FunctionType::get(voidTy, noArgsTy, false);

  llvm::InlineAsm *mov_rdi = nullptr;

  if (llvm::isa<llvm::ConstantInt>(exit_code)) {
    if (auto error = llvm::InlineAsm::verify(mov_rdi_type, "={rdi},i")) {
      std::string errstr = "constraint code for mov_rdi_type not valid: " +
                           LLVMErrorToString(error);
      FatalError(errstr.data(), __FILE__, __LINE__);
    }

    mov_rdi = llvm::InlineAsm::get(
        mov_rdi_type, "mov rdi, $1",
        "={rdi},i", // this says the instruction writes an argument,
                    // which is an immediate integer, to rdi
        true,       // hasSideEffects
        false,      // isAlignStack
        llvm::InlineAsm::AsmDialect::AD_Intel,
        false); // canThrow

  } else {
    if (auto error = llvm::InlineAsm::verify(mov_rdi_type, "={rdi},r")) {
      std::string errstr = "constraint code for mov_rdi_type not valid " +
                           LLVMErrorToString(error);
      FatalError(errstr.data(), __FILE__, __LINE__);
    }

    mov_rdi = llvm::InlineAsm::get(
        mov_rdi_type, "mov rdi, $1",
        "={rdi},r", // this says the instruction writes an argument,
                    // which is a register, to rdi
        true,       // hasSideEffects
        false,      // isAlignStack
        llvm::InlineAsm::AsmDialect::AD_Intel,
        false); // canThrow
  }

  if (auto error = llvm::InlineAsm::verify(mov_rax_type, "={rax}")) {
    std::string errstr = "constraint code for mov_rax_type not valid " +
                         LLVMErrorToString(error);
    FatalError(errstr.data(), __FILE__, __LINE__);
  }

  if (auto error = llvm::InlineAsm::verify(syscall_type, "")) {
    std::string errstr = "constraint code for syscall_type not valid " +
                         LLVMErrorToString(error);
    FatalError(errstr.data(), __FILE__, __LINE__);
  }

  llvm::InlineAsm *mov_rax = llvm::InlineAsm::get(
      mov_rax_type, "mov rax, 60",
      "={rax}", // this says the instruction writes an immediate int to rax
      true,     // hasSideEffects
      false,    // isAlignStack
      llvm::InlineAsm::AsmDialect::AD_Intel,
      false); // canThrow

  llvm::InlineAsm *syscall = llvm::InlineAsm::get(
      syscall_type, "syscall",
      "",    // syscall uses no data, and does not return in this case,
             // other times it's return value is within rax.
      true,  // hasSideEffect
      false, // isAlignStack
      llvm::InlineAsm::AsmDialect::AD_Intel,
      false); // canThrow

  Outcome<llvm::Value *, Error> cast_result = Cast(exit_code, int64Ty, env);
  assert(cast_result);

  env.instruction_builder->CreateCall(mov_rdi_type, mov_rdi,
                                      {cast_result.GetFirst()});
  env.instruction_builder->CreateCall(mov_rax_type, mov_rax);
  env.instruction_builder->CreateCall(syscall_type, syscall);
}

} // namespace pink