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
#include "llvm/Passes/PassBuilder.h" // llvm::PassBuilder

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
   * everything up to that point. if there is no '.' in the
   * input string, then the entire string after the last '/ is
   * returned.
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
