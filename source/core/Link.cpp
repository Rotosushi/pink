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

#include "llvm/Support/raw_os_ostream.h"

#include "lld/Common/Driver.h"

#include "support/FatalError.h"

#include "core/Link.h"

namespace pink {
/**
 * @brief Links together the object file represented by
 * this Environment to construct an executable file.
 *
 * \todo what do we do when we want to translate multiple
 * files into a single executable? that is when the Environment
 * no longer represents just one translation unit, but many?
 * for a start, we emit llvm files as 'object' files, and
 * let lld compile them, this also turns on Link Time Optimization.
 * the only other problem is import/export of symbols and types,
 * which involves llvm forward declarations.
 *
 * @param env
 */
auto Link(std::ostream &out, std::ostream &err, const CompilationUnit &env)
    -> int {
  // #RULE once we link we clean up the object file
  auto remove_object_file = [&env]() {
    std::error_code errc;
    if (!fs::remove(env.GetObjectFile(), errc)) {
      FatalError(errc);
    }
  };

  if (!fs::exists(env.GetObjectFile())) {
    err << "No object file [" << env.GetObjectFile() << "] exists to link.";
    return EXIT_FAILURE;
  }

  llvm::raw_os_ostream      llvm_err = err;
  llvm::raw_os_ostream      llvm_out = out;
  std::vector<const char *> lld_args = {"lld",
                                        "-m",
                                        "elf_x86_64",
                                        "--entry",
                                        "main",
                                        env.GetObjectFile().c_str(),
                                        "-o",
                                        env.GetExecutableFile().c_str()};

  if (!lld::elf::link(lld_args,
                      llvm_out,
                      llvm_err,
                      /* exitEarly */ false,
                      /* disableOutput */ false)) {
    remove_object_file();
    return EXIT_FAILURE;
  }
  remove_object_file();
  return EXIT_SUCCESS;
}

} // namespace pink
