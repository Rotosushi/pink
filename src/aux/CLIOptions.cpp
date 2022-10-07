#include <getopt.h>

#include "PinkConfig.h"
#include "aux/CLIOptions.h"
#include "aux/Error.h"

namespace pink {
CLIOptions::CLIOptions()
    : verbose(false), emit_assembly(false), emit_object(true), emit_llvm(false),
      link(true), optimization_level(llvm::OptimizationLevel::O0)

{}

CLIOptions::CLIOptions(std::string infile, std::string outfile, bool verbose,
                       bool emit_asm, bool emit_obj, bool emit_llvm, bool link,
                       llvm::OptimizationLevel optimization_level)
    : input_file(std::move(infile)), output_file(std::move(outfile)),
      verbose(verbose), emit_assembly(emit_asm), emit_object(emit_obj),
      emit_llvm(emit_llvm), link(link), optimization_level(optimization_level) {
}

auto CLIOptions::RemoveTrailingExtensions(std::string filename) -> std::string {
  auto first_extension = filename.end();
  std::string search_string("/");

  // find the -last- occurance of "/" in the filename
  auto last_directory =
      std::find_end(filename.begin(), filename.end(), search_string.begin(),
                    search_string.end());

  if (last_directory ==
      filename.end()) { // if "/" did not appear in the filename
    first_extension = std::find(filename.begin(), filename.end(), '.');
  } else { // "/" did appear in the filename
    first_extension = std::find(last_directory, filename.end(), '.');
  }

  // if std::find did not find '.' then this call becomes equivalent to
  // return std::string(filename.begin(), filename.end());
  return {filename.begin(), first_extension};
}

auto PrintVersion(std::ostream &out) -> std::ostream & {
  out << "pink version: " << pink_VERSION_MAJOR << "." << pink_VERSION_MINOR
      << std::endl;
  return out;
}

auto PrintHelp(std::ostream &out) -> std::ostream & {
  out << "pink Usage: \n"
      << "General program options: \n"
      << "-h, --help: print this help message and exit.\n"
      << "-v, --version: print version information and exit.\n"
      << "--verbose: set verbose program output (defaults to brief output).\n"
      << "--brief: reset verbose program output (this is the default "
         "behavior).\n"
      << "-i <arg>, --infile <arg>, --input <arg>: specifies the input source "
         "filename.\n"
      << "-o <arg>, --outfile <arg>, --output <arg>: specifies the output "
         "filename.\n"
      << "-O <arg>, --optimize <arg>: specifies the optimization level to "
         "use.\n\t valid arguments are:\n\t\t 0 (none),\n\t\t 1 "
         "(limited),\n\t\t 2 (regular),\n\t\t 3 (high, may affect compile "
         "times),\n\t\t s (small code size), \n\t\t z (very small code size, "
         "increases run times)\n"
      << "-L --emit-only-llvm: emit llvm IR instead of an executable\n"
      << "-l --emit-llvm:      emit llvm IR in addition to an executable\n\n"
      << "-S --emit-only-asm:  emit target assembly instead of an executable\n"
      << "-s --emit-asm:       emit target assembly in addition to an "
         "executable\n\n"
      << "-C --emit-only-obj:  emit an object file instead of an executable\n"
      << "-c --emit-obj:       emit an object file instead of an executable\n\n"
      << "\n";
  return out;
}

auto ParseCLIOptions(std::ostream &out, int argc, char **argv)
    -> std::shared_ptr<CLIOptions> {
  unsigned numopt = 0; // count of how many options we parsed
  const char *short_options = "hvi:o:O:lcs";

  std::string input_file;
  std::string output_file;
  static int verbose = 0;
  bool emit_llvm = false;
  bool emit_asm = false;
  bool emit_obj = true;
  llvm::OptimizationLevel optimization_level = llvm::OptimizationLevel::O0;
  bool link = true;

  // note: we have to use a c style array here because of the api of
  // getopt_long(3) NOLINTBEGIN(modernize-avoid-c-arrays)
  static struct option long_options[] = {
      {"verbose", no_argument, &verbose,
       1}, // getopt_long returns zero in this case
      {"brief", no_argument, &verbose, 0},
      {"help", no_argument, nullptr, 'h'},
      {"version", no_argument, nullptr, 'v'},
      {"infile", required_argument, nullptr, 'i'},
      {"input", required_argument, nullptr, 'i'},
      {"outfile", required_argument, nullptr, 'o'},
      {"output", required_argument, nullptr, 'o'},
      {"optimize", required_argument, nullptr, 'O'},
      {"emit-llvm", no_argument, nullptr, 'l'},
      {"emit-asm", no_argument, nullptr, 's'},
      {"emit-obj", no_argument, nullptr, 'c'},
      {nullptr, 0, nullptr, 0}};
  // NOLINTEND(modernize-avoid-c-arrays)

  int option = 0;
  int optind = 0;

  while (true) {
    option = getopt_long(argc, argv, short_options, long_options, &optind);

    if (option == -1) {
      break; // end of options.
    }

    // we are about to handle an option, so up the count by one
    numopt += 1;

    switch (option) {
    case 0: {
      // this option set a flag, so has already been handled
      break;
    }

    case 'h': {
      PrintHelp(out);
      exit(0);
      break;
    }

    case 'v': {
      PrintVersion(out);
      exit(0);
      break;
    }

    case 'i': {
      input_file = optarg;
      break;
    }

    case 'o': {
      output_file = optarg;
      break;
    }

    case 'l': {
      emit_llvm = true;
      emit_obj = false;
      emit_asm = false;
      link = false;
      break;
    }

    case 'c': {
      emit_obj = true;
      emit_llvm = false;
      emit_asm = false;
      link = false;
      break;
    }

    case 's': {
      emit_asm = true;
      emit_obj = false;
      emit_llvm = false;
      link = false;
      break;
    }

    case 'O': {
      switch (optarg[0]) {
      case '0': {
        optimization_level = llvm::OptimizationLevel::O0;
        break;
      }

      case '1': {
        optimization_level = llvm::OptimizationLevel::O1;
        break;
      }

      case '2': {
        optimization_level = llvm::OptimizationLevel::O2;
        break;
      }

      case '3': {
        optimization_level = llvm::OptimizationLevel::O3;
        break;
      }

      case 's': {
        optimization_level = llvm::OptimizationLevel::Os;
        break;
      }

      case 'z': {
        optimization_level = llvm::OptimizationLevel::Oz;
        break;
      }

      default: {
        FatalError("Option: " + std::string(optarg) +
                       " is not a valid optimization level, use one of: 0, 1, "
                       "2, 3, s, z",
                   __FILE__, __LINE__);
      }
      }
      break;
    }

    case '?': {
      // the unknown option case, getopt_long printed an error message already
      break;
    }

    default: {
      FatalError("Error while parsing option: " + std::string(optarg), __FILE__,
                 __LINE__);
      break;
    }
    }
  }

  // #NOTE: 9/16/2022
  // if input_file is still empty after parsing
  // all available options, then we assume there is at
  // least one more option, which is the input file.
  // (except for handling multiple source files this
  //  is the default behavior of many compilers, any
  //  options appearing on the command line not associated
  //  with any particualr option are considered to be input
  //  files to the compiler.)
  //  this check allows the program to be called like:
  //  ->	pink input_file.p
  //  and the default behavior is to emit an executable
  //  named:
  //  -> input_file
  //
  if (input_file.empty()) {
    // optind is now the index of the first
    // argv element that is not an option.
    // and is not the name of the program being called
    if (optind >= argc || (optind + 1 + numopt) >= static_cast<size_t>(argc)) {
      FatalError("input file is required", __FILE__, __LINE__);
    }

    // getopt's manual states that the procedure permutes argv
    // as it parses the arguments, that is true, it also says
    // that when it is done parsing it will return -1 and the
    // optind will be set to the first argument that is not an
    // option within argv. This is also true. However, the first
    // argument within argv that is not an option is the first
    // argument within argv, the name of the program being run
    // itself. Which means to get to the names of the files to be
    // compiled, which are being given on the command line, we have
    // to traverse to the other end of argv, to the other side of
    // all of the options that getopt parsed. This is why we add
    // optind to numopt. however, we also need to account for one
    // more option, the first one
    char *option = argv[optind + numopt + 1];
    if (option != nullptr) {
      input_file = option; // assume that option is the name of an input file.
                           // this is okay, because this is the intended
                           // command line usage of this program.
      // #TODO: input_file needs to be a std::vector<std::string> and we
      // need to collect multiple input files together into a single output
      // program. However, that is a few steps ahead of where we are right at
      // the moment.
    } else {
      FatalError("input file is required", __FILE__, __LINE__);
    }
  }

  // give a default name to the output file.
  if (output_file.empty()) {
    // given a filename like "/my/favorite/directory/my_file.p"
    // output_file will equal "/my/favorite/directory/my_file"
    output_file = CLIOptions::RemoveTrailingExtensions(input_file);
  }

  return std::make_shared<CLIOptions>(input_file, output_file, verbose == 1,
                                      emit_asm, emit_obj, emit_llvm, link,
                                      optimization_level);
}

auto CLIOptions::GetExeFilename() const -> std::string { return output_file; }

auto CLIOptions::GetObjFilename() const -> std::string {
  return output_file + ".o";
}

auto CLIOptions::GetAsmFilename() const -> std::string {
  return output_file + ".s";
}

auto CLIOptions::GetLLVMFilename() const -> std::string {
  return output_file + ".ll";
}
} // namespace pink
