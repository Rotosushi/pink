#include <sstream>
#include <system_error> // std::error_code

#include "support/EmitFile.h"

#include "aux/Error.h"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h" // llvm::raw_fd_stream

namespace pink {
void EmitObjectFile(const Environment &env, const std::string &filename) {
  std::error_code outfile_error;
  llvm::raw_fd_ostream outfile(filename, outfile_error);

  if (outfile_error) {
    std::stringstream error_message;
    error_message << outfile_error;
    pink::FatalError("Could not open output file [" + filename +
                         "] because of an error: " + error_message.str(),
                     __FILE__, __LINE__);
  }

  llvm::legacy::PassManager objPrintPass;
  if (env.target_machine->addPassesToEmitFile(
          objPrintPass, outfile, nullptr,
          llvm::CodeGenFileType::CGFT_ObjectFile)) {
    FatalError("the target machine " +
                   env.target_machine->getTargetTriple().str() +
                   " cannot emit an object file of this type",
               __FILE__, __LINE__);
  }

  objPrintPass.run(*env.module);

  outfile.close();
}

void EmitAssemblyFile(const Environment &env, const std::string &filename) {
  std::error_code outfile_error;
  llvm::raw_fd_ostream outfile(filename, outfile_error);

  if (outfile_error) {
    std::stringstream error_message;

    error_message << outfile_error;
    pink::FatalError("Could not open output file [" + filename +
                         "] because of an error: " + error_message.str(),
                     __FILE__, __LINE__);
  }

  llvm::legacy::PassManager asmPrintPass;
  // addPassesToEmitFile(legacy::PassManager pass_manager,
  //                     llvm::raw_fd_ostream& outfile,
  //                     llvm::raw_fd_ostream* dwarfOutfile,
  //                     llvm::CodeGenFileType CodeGenFileType,)
  //
  // notice the nullptr will need to be replaced with a pointer to the
  // dwarf object output file, once we emit debugging information.
  if (env.target_machine->addPassesToEmitFile(
          asmPrintPass, outfile, nullptr,
          llvm::CodeGenFileType::CGFT_AssemblyFile)) {
    FatalError("the target_machine " +
                   env.target_machine->getTargetTriple().str() +
                   " cannot emit an assembly file of this type",
               __FILE__, __LINE__);
  }

  // this step fills 'outstream' with the contents of the module,
  // that is, it writes the contents of the module to the file.
  asmPrintPass.run(*env.module);

  if (outfile.has_error()) {
    std::stringstream errmsg;
    errmsg << outfile.error();
    pink::FatalError("The output stream encountered an error: " + errmsg.str(),
                     __FILE__, __LINE__);
  } else {
    outfile.close();
  }
}

void EmitLLVMFile(const Environment &env, const std::string &filename) {
  std::error_code outfile_error;
  llvm::raw_fd_ostream outfile(
      filename,
      outfile_error); // using a llvm::raw_fd_stream for llvm::Module::Print

  if (outfile_error) {
    std::stringstream error_message;

    error_message << outfile_error;
    pink::FatalError("Could not open output file [" + filename +
                         "] because of an error: " + error_message.str(),
                     __FILE__, __LINE__);
  }

  // simply print to the output file the
  // code generated, as it is already in llvm IR
  env.module->print(outfile, nullptr);

  outfile.close();
}
} // namespace pink