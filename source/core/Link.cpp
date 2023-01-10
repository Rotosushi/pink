
#include "llvm/Support/raw_os_ostream.h"

#include "lld/Common/Driver.h"

#include "core/Link.h"

namespace pink {

/**
 * @brief Links together the object file represented by
 * this Environment to construct an executable file.
 *
 * \todo what do we do when we want to translate multiple
 * files into a single executable? that is when the Environment
 * no longer represents just one translation unit, but many?
 * Obviously the shape of the Environment structure itself must
 * change, but how?
 * first guess: hold a std::vector of translation units, one for each
 * file. A Translation Unit is then all of the data structures
 * which may not be shared among Translation Units; parser,
 * false_bindings, flags, options?, symbols, operators, types,
 * bindings, binops, unops, llvm::LLVMContext, llvm::Module,
 * llvm::InstructionBuilder, current_function), all of these
 * structures can only represent a single translation unit.
 * whereas the llvm::Target, llvm::TargetMachine, llvm::DataLayout
 * are going to be the same for a given call of the compiler.
 * (unless we wanted to allow cross compiling only certain files
 * passed to the compiler on the command line and native compiling
 * the rest of the files, but that feels weird and unecessary, as
 * the end user can simply call the compiler multiple times to solve
 * that problem (admittedly a made up problem),
 * so then whatever we wanted to pass to Link would be the structure
 * holding a reference to all of the Translation Units necessary to
 * create an entire executable. Whereas each Translation Unit would
 * be enough information to compile a single source file into an object file.
 *
 * @param env
 */
auto Link(std::ostream &out, std::ostream &err, const Environment &env) -> int {
  auto objoutfilename = env.options->output_file + ".o";
  auto exeoutfilename = env.options->output_file + "";

  if (!env.options->link) {
    return EXIT_SUCCESS;
  }

  if (!env.options->emit_object) {
    err << "No object file [" << env.options->output_file + ".o"
        << "] exists to link.";
    return EXIT_FAILURE;
  }

  llvm::raw_os_ostream llvm_err = err;
  llvm::raw_os_ostream llvm_out = out;
  std::vector<const char *> lld_args = {"lld",        "-m",
                                        "elf_x86_64", "--entry",
                                        "main",       objoutfilename.data(),
                                        "-o",         exeoutfilename.data()};

  lld::elf::link(lld_args, llvm_out, llvm_err, /* exitEarly */ false,
                 /* disableOutput */ false);
}

} // namespace pink
