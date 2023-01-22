#include <sstream>
#include <system_error> // std::error_code

#include "support/EmitFile.h"

#include "aux/Error.h"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h" // llvm::raw_fd_stream

namespace pink {
auto EmitObjectFile(std::ostream &out, const Environment &env,
                    const std::string &filename) -> int;

auto EmitAssemblyFile(std::ostream &out, const Environment &env,
                      const std::string &filename) -> int;

auto EmitLLVMFile(std::ostream &out, const Environment &env,
                  const std::string &filename) -> int;

auto EmitFiles(std::ostream &out, const Environment &env) -> int {
  const auto &options = env.options;
  int result = EXIT_SUCCESS;
  if (options->emit_llvm) {
    result &= EmitLLVMFile(out, env, options->GetLLVMFilename());
    if (result == EXIT_FAILURE) {
      return result;
    }
  }

  if (options->emit_assembly) {
    result &= EmitAssemblyFile(out, env, options->GetAsmFilename());
    if (result == EXIT_FAILURE) {
      return result;
    }
  }

  if (options->emit_object) {
    result &= EmitObjectFile(out, env, options->GetObjFilename());
    if (result == EXIT_FAILURE) {
      return result;
    }
  }

  return result;
}

auto EmitObjectFile(std::ostream &out, const Environment &env,
                    const std::string &filename) -> int {
  std::error_code outfile_error;
  llvm::raw_fd_ostream outfile(filename, outfile_error);

  if (outfile_error) {
    std::stringstream error_message;
    error_message << outfile_error;
    out << "Could not open output file [" << filename
        << "] error: " << error_message.str();
    return EXIT_FAILURE;
  }

  llvm::legacy::PassManager objPrintPass;
  if (env.target_machine->addPassesToEmitFile(
          objPrintPass, outfile, nullptr,
          llvm::CodeGenFileType::CGFT_ObjectFile)) {
    out << "the target machine " << env.target_machine->getTargetTriple().str()
        << " cannot emit an object file of this type";
    return EXIT_FAILURE;
  }

  objPrintPass.run(*env.llvm_module);
  return EXIT_SUCCESS;
}

auto EmitAssemblyFile(std::ostream &out, const Environment &env,
                      const std::string &filename) -> int {
  std::error_code outfile_error;
  llvm::raw_fd_ostream outfile(filename, outfile_error);

  if (outfile_error) {
    std::stringstream error_message;
    error_message << outfile_error;
    out << "Could not open output file [" << filename
        << "] because of an error: " << error_message.str();
    return EXIT_FAILURE;
  }

  llvm::legacy::PassManager asmPrintPass;
  if (env.target_machine->addPassesToEmitFile(
          asmPrintPass, outfile, nullptr,
          llvm::CodeGenFileType::CGFT_AssemblyFile)) {
    out << "the target_machine " << env.target_machine->getTargetTriple().str()
        << " cannot emit an assembly file of this type";
    return EXIT_FAILURE;
  }

  return asmPrintPass.run(*env.llvm_module);
}

auto EmitLLVMFile(std::ostream &out, const Environment &env,
                  const std::string &filename) -> int {
  std::error_code outfile_error;
  llvm::raw_fd_ostream outfile(filename, outfile_error);

  if (outfile_error) {
    std::stringstream error_message;
    error_message << outfile_error;
    out << "Could not open output file [" << filename
        << "] because of an error: " << error_message.str();
    return EXIT_FAILURE;
  }

  env.llvm_module->print(outfile, nullptr);
  return EXIT_SUCCESS;
}
} // namespace pink