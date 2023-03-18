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

/**
 * @file CLIOptions.h
 * @brief Header for class CLIOptions
 * @version 0.1
 *
 */
#pragma once
#include <bitset>
#include <iostream>
#include <memory>
#include <string>

#include "llvm/Passes/OptimizationLevel.h"

#include "support/BitsPerByte.h"

namespace pink {
class CLIFlags {
private:
  enum Flags {
    verbose,
    emit_assembly,
    emit_object,
    link,
    SIZE, // #NOTE! this -must- be the last member,
          // no enums can have an assigned value.
  };
  static constexpr auto bitset_size = Flags::SIZE;
  using Set                         = std::bitset<bitset_size>;

  Set set;

public:
  CLIFlags() {
    set[emit_object] = true;
    set[link]        = true;
  }

  auto DoVerbose(bool state) noexcept -> bool { return set[verbose] = state; }
  [[nodiscard]] auto DoVerbose() const noexcept -> bool { return set[verbose]; }

  auto DoEmitAssembly(bool state) noexcept -> bool {
    return set[emit_assembly] = state;
  }
  [[nodiscard]] auto DoEmitAssembly() const noexcept -> bool {
    return set[emit_assembly];
  }

  auto DoEmitObject(bool state) noexcept -> bool {
    return set[emit_object] = state;
  }
  [[nodiscard]] auto DoEmitObject() const noexcept -> bool {
    return set[emit_object];
  }

  auto DoLink(bool state) noexcept -> bool { return set[link] = state; }
  [[nodiscard]] auto DoLink() const noexcept -> bool { return set[link]; }
};

/**
 * @brief Holds all of the Options which the user can manipulate via the command
 * line.
 *
 */
class CLIOptions {
private:
  std::string             input_file;
  std::string             output_file;
  std::string             assembly_file;
  std::string             object_file;
  CLIFlags                flags;
  llvm::OptimizationLevel optimization_level;

public:
  CLIOptions()
      : optimization_level(llvm::OptimizationLevel::O0) {}
  // we can lazily construct assembly_file and object_file
  // here for minor savings in the cases where we are not
  // generating assembly or object files.
  CLIOptions(std::string_view        infile,
             std::string_view        outfile,
             CLIFlags                flags,
             llvm::OptimizationLevel optimization_level)
      : input_file{infile},
        output_file{outfile},
        assembly_file{output_file + ".s"},
        object_file{output_file + ".ll"},
        flags{flags},
        optimization_level{optimization_level} {}
  ~CLIOptions() noexcept                                           = default;
  CLIOptions(const CLIOptions &other) noexcept                     = default;
  CLIOptions(CLIOptions &&other) noexcept                          = default;
  auto operator=(const CLIOptions &other) noexcept -> CLIOptions & = default;
  auto operator=(CLIOptions &&other) noexcept -> CLIOptions      & = default;

  /**
   * @brief Remove trailing filename extensions
   *
   * searches for the first '.' appearing after the last '/' in
   * the given input string and returns the string containing
   * everything up to that point. if  no '.' is found then the
   * entire string is returned.
   *
   * @param filename the filename to remove the extensions of
   * @return std::string the new filename with no extensions
   */
  static auto RemoveTrailingExtensions(std::string_view filename)
      -> std::string;

  static auto PrintVersion(std::ostream &out) -> std::ostream &;
  static auto PrintHelp(std::ostream &out) -> std::ostream &;

  [[nodiscard]] auto DoEmitAssembly() const noexcept -> bool {
    return flags.DoEmitAssembly();
  }

  [[nodiscard]] auto DoEmitObject() const noexcept -> bool {
    return flags.DoEmitObject();
  }

  [[nodiscard]] auto DoLink() const noexcept -> bool { return flags.DoLink(); }

  [[nodiscard]] auto DoVerbose() const noexcept -> bool {
    return flags.DoVerbose();
  }

  [[nodiscard]] auto GetInputFilename() const -> std::string_view {
    return input_file;
  }

  [[nodiscard]] auto GetExecutableFilename() const -> std::string_view {
    return output_file;
  }
  [[nodiscard]] auto GetObjectFilename() const -> std::string_view {
    return object_file;
  }

  [[nodiscard]] auto GetAssemblyFilename() const -> std::string_view {
    return assembly_file;
  }

  [[nodiscard]] auto GetOptimizationLevel() const -> llvm::OptimizationLevel {
    return optimization_level;
  }
};

auto ParseCLIOptions(std::ostream &out, int argc, char **argv) -> CLIOptions;

} // namespace pink
