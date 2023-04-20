// Copyright (C) 2023 Cade Weinberg
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
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <random>
#include <sstream>

namespace fs = std::filesystem;

#include "support/FatalError.h"

/*
  steps:
    1) construct a valid main file which returns some known value
    2) emit the main file into a temp file
    3) compile the temp file with pink
    4) run the emitted program
    5) check that the returned value equals the known value
    6) repeat for each test.
*/

[[maybe_unused]] static fs::path CreateUniqueTempFilename() {
  auto temp_path = []() {
    std::error_code errc;
    fs::path        temp_path = fs::temp_directory_path(errc);
    if (errc) {
      pink::FatalError(errc);
    }
    return temp_path;
  };

  auto temp_filename = []() {
    std::random_device seed;
    std::stringstream  sstream;
    sstream << "temp" << seed();
    return sstream.str();
  };

  auto temp_filepath = [&]() { return temp_path() / temp_filename(); };

  auto unique_tempfile = [&]() {
    fs::path temp_file = temp_filepath();
    while (fs::exists(temp_file)) {
      temp_file = temp_filepath();
    }
    return temp_file;
  };

  return unique_tempfile();
}

[[maybe_unused]] static void EmitTempFile(const std::string &contents,
                                          const fs::path    &tempfile) {
  std::fstream file{tempfile};
  if (!file.is_open()) {
    std::string errmsg{"Unable to open file "};
    errmsg += tempfile;
    pink::FatalError(errmsg);
  }

  file << contents;
}
