

#include "aux/Environment.h"

#include "runtime/ops/BinopPrimitives.h"
#include "runtime/ops/UnopPrimitives.h"

#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"

#include "llvm/MC/TargetRegistry.h"

#include "llvm/Target/TargetMachine.h"

#include "llvm/Passes/OptimizationLevel.h"

#include "llvm/IR/LegacyPassManager.h"

namespace pink {
auto Environment::Gensym(std::string_view prefix) -> InternedString {
  auto generate = [&]() {
    std::string sym{prefix};
    sym += std::to_string(gensym_counter);
    sym += "__";
    return sym;
  };
  const auto *candidate{variable_interner.Intern(generate())};
  while (scopes.LookupLocal(candidate).has_value()) {
    candidate = variable_interner.Intern(generate());
  }
  return candidate;
}

auto Environment::EmitFiles(std::ostream &err) const -> int {
  if (cli_options.DoEmitObject()) {
    if (EmitObjectFile(err) == EXIT_FAILURE) {
      return EXIT_FAILURE;
    }
  }

  if (cli_options.DoEmitAssembly()) {
    if (EmitAssemblyFile(err) == EXIT_FAILURE) {
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

auto Environment::EmitObjectFile(std::ostream &err) const -> int {
  auto                 filename = cli_options.GetObjectFilename();
  std::error_code      outfile_error;
  llvm::raw_fd_ostream outfile{filename, outfile_error};
  if (outfile_error) {
    err << "Couldn't open output file [" << filename << "] " << outfile_error
        << "\n";
    return EXIT_FAILURE;
  }

  module->print(outfile, nullptr);
  return EXIT_SUCCESS;
}

auto Environment::EmitAssemblyFile(std::ostream &err) const -> int {
  auto                 filename = cli_options.GetAssemblyFilename();
  std::error_code      outfile_error{};
  llvm::raw_fd_ostream outfile{filename, outfile_error};
  if (outfile_error) {
    err << "Couldn't open output file [" << filename << "]" << outfile_error
        << "\n";
    return EXIT_FAILURE;
  }

  llvm::legacy::PassManager AssemblyPrinter;
  auto                      failed{target_machine->addPassesToEmitFile(
      AssemblyPrinter,
      outfile,
      nullptr,
      llvm::CodeGenFileType::CGFT_AssemblyFile)};

  if (failed) {
    err << "Target machine [" << target_machine->getTargetCPU().data() << ","
        << target_machine->getTargetTriple().str()
        << "] cannot write an assembly file.\n";
    return EXIT_FAILURE;
  }
  AssemblyPrinter.run(module.operator*());
  return EXIT_SUCCESS;
}

auto Environment::NativeCPUFeatures() noexcept -> std::string {
  std::string           cpu_features;
  llvm::StringMap<bool> features;

  if (llvm::sys::getHostCPUFeatures(features)) {
    auto     idx      = features.begin();
    auto     end      = features.end();
    unsigned numElems = features.getNumItems();
    unsigned jdx      = 0;

    while (idx != end) {
      if ((*idx).getValue()) {
        cpu_features += std::string("+");
        cpu_features += idx->getKeyData();
      } else {
        cpu_features += std::string("-");
        cpu_features += idx->getKeyData();
      }

      if (jdx < (numElems - 1)) {
        cpu_features += ",";
      }

      idx++;
      jdx++;
    }
  }

  return cpu_features;
}

auto Environment::CreateNativeEnvironment(CLIOptions cli_options)
    -> Environment {
  // llvm::InitializeAllTargetInfos();
  // llvm::InitializeAllTargets();
  // llvm::InitializeAllTargetMCs();
  // llvm::InitializeAllAsmPrinters();
  // llvm::InitializeAllAsmParsers();
  // llvm::InitializeAllDisassemblers();
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();
  llvm::InitializeNativeTargetDisassembler();

  auto        context       = std::make_unique<llvm::LLVMContext>();
  std::string target_triple = llvm::sys::getProcessTriple();

  std::string         error;
  const llvm::Target *target =
      llvm::TargetRegistry::lookupTarget(target_triple, error);
  if (target == nullptr) {
    FatalError(error.data(), __FILE__, __LINE__);
  }

  auto *target_machine =
      target->createTargetMachine(target_triple,
                                  llvm::sys::getHostCPUName().str(),
                                  NativeCPUFeatures(),
                                  llvm::TargetOptions{},
                                  llvm::Reloc::Model::PIC_,
                                  llvm::CodeModel::Model::Small);
  auto data_layout = target_machine->createDataLayout();

  auto instruction_builder = std::make_unique<llvm::IRBuilder<>>(*context);
  auto module =
      std::make_unique<llvm::Module>(cli_options.GetInputFilename(), *context);
  module->setSourceFileName(cli_options.GetInputFilename());
  module->setDataLayout(data_layout);
  module->setTargetTriple(target_triple);

  Environment env{std::move(cli_options),
                  std::move(context),
                  std::move(module),
                  std::move(instruction_builder),
                  target_machine};

  InitializeBinopPrimitives(env);
  InitializeUnopPrimitives(env);

  return env;
}

auto Environment::CreateNativeEnvironment() -> Environment {
  return CreateNativeEnvironment(CLIOptions{});
}
} // namespace pink
