#include "kernel/RuntimeError.h"
#include "kernel/AllocateText.h"
#include "kernel/SysExit.h"
#include "kernel/SysWrite.h"

#include "support/Gensym.h"

namespace pink {
// write the given error description to stderr and then exit the process.
// \todo This works fine, except that it repeats the string allocation on each
// call to this function. so if we emit many of the same RuntimeError,
// we duplicate the string passed. What could we use as a solution instead?
// my first guess is to keep track of all possible RuntimeErrors, (the only
// one we currently support is 'index out of bounds') and then
// only allocate the strings if they are actually used. Then whenever we
// emit the same RuntimeError we can use the same error description each time.
// This however does not natively support the ability to construct contextual
// information into the error string produced. To do that we must allocate the
// contextual information as well, and the RuntimeError mechanism must
// emit both strings. (similar to how we handle Syntax/Type/Semantic Errors.)
void RuntimeError(const std::string &error_description, llvm::Value *exit_code,
                  const Environment &env) {
  auto *error_string = AllocateGlobalText(Gensym(), error_description, env);
  auto size = error_description.size() + 1;
  auto *character_type = env.types->GetCharacterType();
  auto *string_type = llvm::cast<llvm::StructType>(
      env.types->GetArrayType(size, character_type)->ToLLVM(env));
  auto *stderr_fd = env.instruction_builder->getInt64(2);
  SysWriteText(stderr_fd, string_type, error_string, env);
  SysExit(exit_code, env);
}
} // namespace pink