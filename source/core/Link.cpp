
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
void Link(const Environment &env) {
  std::string objoutfilename = env.options->output_file + ".o";
  std::string exeoutfilename = env.options->output_file + "";

  llvm::raw_os_ostream std_err = std::cerr;
  llvm::raw_os_ostream std_out = std::cout;

  std::vector<const char *> lld_args = {"lld",        "-m",
                                        "elf_x86_64", "--entry",
                                        "main",       objoutfilename.data(),
                                        "-o",         exeoutfilename.data()};

  lld::elf::link(lld_args, std_out, std_err, /* exitEarly */ false,
                 /* disableOutput */ false);

  // #NOTE: we may alternatively use std::system instead of
  // statically linking against lld. however the static linking
  // creates a standalone executable, which is nice.
  // We may also dynamically link with lld, which only loads
  // lld if and when we link something; which is what is more
  // performant in the cases where we don't create an executable
  // from the source. (when we emit llvm IR or object files)
}

} // namespace pink
