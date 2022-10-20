#include "kernel/RuntimeError.h"
#include "kernel/AllocateString.h"
#include "kernel/Gensym.h"
#include "kernel/SysExit.h"
#include "kernel/SysWrite.h"

namespace pink {
// write the given error description to stderr and then exit the process.
void RuntimeError(const std::string &error_description, llvm::Value *exit_code,
                  const Environment &env) {
  auto *error_string = AllocateGlobalString(Gensym(), error_description, env);
  auto *stderr_fd = env.instruction_builder->getInt64(2);
  SysWrite(stderr_fd, error_string, env);
  SysExit(exit_code, env);
}
} // namespace pink