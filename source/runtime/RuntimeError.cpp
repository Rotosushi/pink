#include "runtime/RuntimeError.h"
#include "runtime/AllocateText.h"
#include "runtime/sys/SysExit.h"
#include "runtime/sys/SysWrite.h"

#include "type/action/ToLLVM.h"

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
void RuntimeError(const std::string &description,
                  llvm::Value       *exit_code,
                  Environment       &env) {
  assert(exit_code != nullptr);
  auto *error_string{AllocateGlobalText(env.Gensym(), description, env)};
  auto *array_type{env.type_interner.GetTextType(description.size())};
  auto *string_type{llvm::cast<llvm::StructType>(ToLLVM(array_type, env))};
  auto *stderr_fd = env.instruction_builder->getInt64(2);
  SysWriteText(stderr_fd, string_type, error_string, env);
  SysExit(exit_code, env);
}
} // namespace pink