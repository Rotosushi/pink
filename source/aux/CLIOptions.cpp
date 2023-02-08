#include <getopt.h>

#include "PinkConfig.h"
#include "aux/CLIOptions.h"
#include "aux/Error.h"

namespace pink {
auto CLIOptions::RemoveTrailingExtensions(std::string filename) -> std::string {
  auto        first_extension = filename.end();
  std::string search_string("/");
  // find the -last- occurance of "/" in the filename
  auto        last_directory = std::find_end(filename.begin(),
                                      filename.end(),
                                      search_string.begin(),
                                      search_string.end());
  // if "/" did not appear in the filename
  if (last_directory == filename.end()) {
    first_extension = std::find(filename.begin(), filename.end(), '.');
  } else { // "/" did appear in the filename
    first_extension = std::find(last_directory, filename.end(), '.');
  }
  // if std::find did not find '.' then this call becomes equivalent to
  // return std::string(filename.begin(), filename.end());
  return {filename.begin(), first_extension};
}

auto CLIOptions::PrintVersion(std::ostream &out) -> std::ostream & {
  out << "pink version: " << pink_VERSION_MAJOR << "." << pink_VERSION_MINOR
      << "\n";
  return out;
}

auto CLIOptions::PrintHelp(std::ostream &out) -> std::ostream & {
  out << "pink Usage: \n"
      << "General program options: \n"
      << "-h, --help: print this help message and exit.\n"
      << "-v, --version: print version information and exit.\n"
      << "-i <arg>, --infile <arg>, --input <arg>: specifies the input source "
         "filename.\n"
      << "-o <arg>, --outfile <arg>, --output <arg>: specifies the output "
         "filename.\n"
      << "-O <arg>, --optimize <arg>: specifies the optimization level to use."
      << "\n\t valid arguments are:"
      << "\n\t\t 0 (none),\n\t\t 1 (limited),\n\t\t 2 (regular),"
      << "\n\t\t 3 (high, may affect compile times),\n\t\t s (small code size),"
      << " \n\t\t z (very small code size at performance cost)\n"
      << "-l --emit-llvm: emit llvm IR instead of an executable\n"
      << "-s --emit-asm: emit assembly instead of an executable\n"
      << "-c --emit-obj: emit an object file instead of an executable\n"
      << "\n";
  return out;
}

/*
  #NOTE: 2/7/2023
    We cannot use getopt.h on windows or mac, if/when
    that becomes an issue we will have to work around
    it. The easy route long term might be to reimplement
    the bare neccessary functionality of getopt_long by
    hand. Then this function can be compiled on whatever
    platform is needed.
*/
auto ParseCLIOptions(std::ostream &out, int argc, char **argv) -> CLIOptions {
  unsigned    numopt        = 0; // count of how many options we parsed
  const char *short_options = "hvi:o:O:lcs";

  std::string             input_file;
  std::string             output_file;
  CLIFlags                flags;
  llvm::OptimizationLevel optimization_level = llvm::OptimizationLevel::O0;

  // the default behavior of the compiler
  flags.EmitObject(true);
  flags.Link(true);

  // note: we have to use a c style array here because of the api of
  // getopt_long(3) NOLINTBEGIN(modernize-avoid-c-arrays)
  static struct option long_options[] = {
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

    numopt += 1;

    switch (option) {
    case 0: {
      // this option set a flag, so has already been handled
      break;
    }

    case 'h': {
      CLIOptions::PrintHelp(out);
      exit(0);
      break;
    }

    case 'v': {
      CLIOptions::PrintVersion(out);
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
      flags.EmitLLVM(true);
      flags.EmitObject(false);
      flags.Link(false);
      break;
    }

    case 'c': {
      flags.EmitObject(true);
      flags.Link(false);
      break;
    }

    case 's': {
      flags.EmitAssembly(true);
      flags.EmitObject(false);
      flags.Link(false);
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
                   __FILE__,
                   __LINE__);
      }
      }
      break;
    }

    case '?': {
      // the unknown option case, getopt_long printed an error message already
      break;
    }

    default: {
      FatalError("Error while parsing option: " + std::string(optarg),
                 __FILE__,
                 __LINE__);
      break;
    }
    }
  }

  if (input_file.empty()) {
    int infile_option_index = optind + 1 + numopt;
    if (optind >= argc || infile_option_index >= argc) {
      FatalError("input file is required", __FILE__, __LINE__);
    }

    char *option = argv[infile_option_index];
    if (option != nullptr) {
      input_file = option;
    } else {
      FatalError("input file is required", __FILE__, __LINE__);
    }
  }

  if (output_file.empty()) {
    output_file = CLIOptions::RemoveTrailingExtensions(input_file);
  }

  return {input_file, output_file, flags, optimization_level};
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
