#include "kernel/SysExit.h"
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
  llvm::Type *int64Ty = env.instruction_builder->getInt64Ty();
  llvm::Type *voidTy = env.instruction_builder->getVoidTy();

  std::vector<llvm::Type *> intArgsTy = {int64Ty};
  std::vector<llvm::Type *> noArgsTy = {};

  auto *iasm0Ty = llvm::FunctionType::get(int64Ty, noArgsTy, false);
  auto *iasm1Ty = llvm::FunctionType::get(int64Ty, intArgsTy, false);
  auto *iasm2Ty = llvm::FunctionType::get(voidTy, noArgsTy, false);

  llvm::InlineAsm *iasm1 = nullptr;

  if (llvm::isa<llvm::ConstantInt>(exit_code)) {
    if (auto error = llvm::InlineAsm::verify(iasm1Ty, "={rdi},i")) {
      std::string errstr =
          "constraint code for iasm1Ty not valid: " + LLVMErrorToString(error);
      FatalError(errstr.data(), __FILE__, __LINE__);
    }

    iasm1 = llvm::InlineAsm::get(
        iasm1Ty, "mov rdi, $1",
        "={rdi},i", // this says the instruction writes an argument,
                    // which is an immediate integer, to rdi
        true,       // hasSideEffects
        false,      // isAlignStack
        llvm::InlineAsm::AsmDialect::AD_Intel,
        false); // canThrow

  } else {
    if (auto error = llvm::InlineAsm::verify(iasm1Ty, "={rdi},r")) {
      std::string errstr =
          "constraint code for iasm1Ty not valid" + LLVMErrorToString(error);
      FatalError(errstr.data(), __FILE__, __LINE__);
    }

    iasm1 = llvm::InlineAsm::get(
        iasm1Ty, "mov rdi, $1",
        "={rdi},r", // this says the instruction writes an argument,
                    // which is a register, to rdi
        true,       // hasSideEffects
        false,      // isAlignStack
        llvm::InlineAsm::AsmDialect::AD_Intel,
        false); // canThrow
  }

  if (auto error = llvm::InlineAsm::verify(iasm0Ty, "={rax}")) {
    std::string errstr =
        "constraint code for iasm0Ty not valid" + LLVMErrorToString(error);
    FatalError(errstr.data(), __FILE__, __LINE__);
  }

  if (auto error = llvm::InlineAsm::verify(iasm2Ty, "")) {
    std::string errstr =
        "constraint code for iasm2Ty not valid" + LLVMErrorToString(error);
    FatalError(errstr.data(), __FILE__, __LINE__);
  }

  llvm::InlineAsm *iasm0 = llvm::InlineAsm::get(
      iasm0Ty, "mov rax, 60",
      "={rax}", // this says the instruction writes an immediate int to rax
      true,     // hasSideEffects
      false,    // isAlignStack
      llvm::InlineAsm::AsmDialect::AD_Intel,
      false); // canThrow

  llvm::InlineAsm *iasm2 = llvm::InlineAsm::get(
      iasm2Ty, "syscall",
      "",    // syscall uses no data, and does not return in this case,
             // other times it's return value is within rax.
      true,  // hasSideEffect
      false, // isAlignStack
      llvm::InlineAsm::AsmDialect::AD_Intel,
      false); // canThrow

  Outcome<llvm::Value *, Error> cast_result = Cast(exit_code, int64Ty, env);
  assert(cast_result);

  std::vector<llvm::Value *> iasm1Args = {cast_result.GetFirst()};

  // \note: initally the numbering of the inline assembly statements
  // followed the order in which we emitted them, however
  // rax is a commonly selected register for use by llvm,
  // and as such llvm selects rax as the place to store the results
  // of common expressions, if llvm uses rax as the result of an expression
  // which computes the exit code this causes the following inline assembly
  // to overwrite the return code when we load the rax register with the exit
  // code if we emit
  // "mov rax 60" before we emit "mov rdi, $1"
  env.instruction_builder->CreateCall(iasm1Ty, iasm1, iasm1Args);
  env.instruction_builder->CreateCall(iasm0Ty, iasm0);
  env.instruction_builder->CreateCall(iasm2Ty, iasm2);
}

} // namespace pink