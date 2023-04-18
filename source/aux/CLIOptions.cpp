// Copyright (C) 2023 cadence
//
// This file is part of pink.
//
// pink is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// pink is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with pink.  If not, see <http://www.gnu.org/licenses/>.

#include <getopt.h>

#include <algorithm>
#include <vector>

#include "PinkConfig.h"

#include "aux/CLIOptions.h"
#include "aux/Error.h"

#include "support/FatalError.h"

namespace pink {
auto CLIOptions::RemoveTrailingExtensions(std::string_view filename)
    -> std::string {
  const char *first_extension = nullptr;
  std::string search_string("/");
  // find the -last- occurance of "/" in the filename
  const auto *last_directory = std::find_end(filename.begin(),
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
  out << "pink version [" << pink_VERSION_MAJOR << "." << pink_VERSION_MINOR
      << "]\n"
      << "compiled on [" << __DATE__ << " @ " << __TIME__ << "]";
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
      << "-c --emit-object: emit llvm IR instead of an executable\n"
      << "-s --emit-asm: emit assembly instead of an executable\n"
      << "\n";
  return out;
}

/*
  #TODO: rewrite this using some other cross platform
  getopt or getopt like function.

  #TODO: this function really needs to return Outcome<CLIOptions, Error>
  to handle the case of bad arguments gracefully. (currently they are
  fatal errors.)
*/
auto ParseCLIOptions(std::ostream &out, int argc, char **argv) -> CLIOptions {
  int         numopt        = 0; // count of how many options we parsed
  const char *short_options = "hvi:o:O:lcs";

  fs::path                input_file;
  fs::path                output_file;
  CLIFlags                flags;
  llvm::OptimizationLevel optimization_level = llvm::OptimizationLevel::O0;

  // note: we have to use a c style array here because of the api of
  // getopt_long(3) NOLINTBEGIN
  static struct option long_options[] = {
      {"help", no_argument, nullptr, 'h'},
      {"version", no_argument, nullptr, 'v'},
      {"infile", required_argument, nullptr, 'i'},
      {"input", required_argument, nullptr, 'i'},
      {"outfile", required_argument, nullptr, 'o'},
      {"output", required_argument, nullptr, 'o'},
      {"optimize", required_argument, nullptr, 'O'},
      {"emit-llvm", no_argument, nullptr, 'l'},
      {"emit-ll", no_argument, nullptr, 'l'},
      {"emit-object", no_argument, nullptr, 'c'},
      {"emit-obj", no_argument, nullptr, 'c'},
      {"emit-assembly", no_argument, nullptr, 's'},
      {"emit-asm", no_argument, nullptr, 's'},
      {nullptr, 0, nullptr, 0}};
  // NOLINTEND

  int option = 0;
  int optind = 0;

  while (true) {
    option = getopt_long(argc,
                         argv,
                         short_options,
                         (struct option *)long_options,
                         &optind);

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
      flags.DoEmitLLVMIR(true);
      flags.DoLink(false);
      break;
    }

    case 'c': {
      flags.DoEmitObject(true);
      flags.DoLink(false);
      break;
    }

    case 's': {
      flags.DoEmitAssembly(true);
      flags.DoLink(false);
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
        std::string errmsg{"Unkown optimization level ["};
        errmsg += (char)option;
        errmsg += "]; Use one of [0, 1, "
                  "2, 3, s, z]";
        FatalError(errmsg);
      }
      }
      break;
    }

    case '?': {
      // the unknown option case, getopt_long printed an error message already
      break;
    }

    default: {
      std::string errmsg{"Unknown option ["};
      errmsg += (char)option;
      errmsg += "]";
      FatalError(errmsg);
      break;
    }
    }
  }

  if (input_file.empty()) {
    int infile_option_index = optind + 1 + numopt;
    if (optind >= argc || infile_option_index >= argc) {
      FatalError("input file is required");
    }

    char *option = argv[infile_option_index];
    if (option != nullptr) {
      input_file = option;
      input_file = fs::absolute(input_file);
    } else {
      FatalError("input file is required");
    }
  }

  if (output_file.empty()) {
    output_file = input_file;
    output_file.replace_extension();
  }

  return {input_file, output_file, flags, optimization_level};
}
} // namespace pink
