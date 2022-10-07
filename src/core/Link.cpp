
#include "llvm/Support/raw_os_ostream.h"

#include "lld/Common/Driver.h"

#include "core/Link.h"

namespace pink {

void Link(const Environment &env) {
  // #TODO: Test that this works when the user passes in a custom output
  // filename.
  std::string objoutfilename = env.options->output_file + ".o";
  std::string exeoutfilename = env.options->output_file + "";

  llvm::raw_os_ostream std_err = std::cerr;
  llvm::raw_os_ostream std_out = std::cout;

  // #TODO find these files in a more dynamic way
  //  #NOTE: even though we do not use these files
  //  in particular it may still be useful to define
  //  a routine which can search the system for the
  //  absolute path of a given file. for dependency
  //  lookup.
  // std::string crt1 = "/usr/lib/x86_64-linux-gnu/crt1.o";
  // std::string crti = "/usr/lib/x86_64-linux-gnu/crti.o";
  // std::string crtbegin = "/usr/lib/gcc/x86_64-linux-gnu/11/crtbegin.o";
  // std::string crt1path = "-L/usr/lib/x86_64-linux-gnu";
  // std::string crtbeginpath = "-L/usr/lib/gcc/x86_64-linux-gnu/11";
  // std::string crtend = "/usr/lib/gcc/x86_64-linux-gnu/11/crtend.o";
  // std::string crtn   = "/usr/lib/x86_64-linux-gnu/crtn.o";

  std::vector<const char *> lld_args({
      "ld.lld-14", "-m", "elf_x86_64", "--entry",
      "main", // instead of linking to the crt to have the crt
              // define the symbol, _start. simply tell the
              // linker that 'main' is the entry point.
              //
      //"-dynamic-linker", "lib64/ld-linux-x86-64.so.2",
      // crt1.data(),
      // crti.data(),
      // crtbegin.data(),
      // crt1path.data(),
      // crtbeginpath.data(),
      //"/usr/lib/x86_64-linux-gnu/libc.so",
      objoutfilename.data(), "-o", exeoutfilename.data()
      // crtend.data(),
      // crtn.data()
  });

  lld::elf::link(lld_args, std_out, std_err, /* exitEarly */ false,
                 /* disableOutput */ false);

  // #NOTE: we may alternatively use std::system instead of
  // statically linking against lld. however the static linking
  // creates a standalone executable, which is nice.
  // int result = std::system(command.data());
}

} // namespace pink
