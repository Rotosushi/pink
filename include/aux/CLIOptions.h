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
  enum Flags : uint8_t {
    emit_assembly,
    emit_object,
    emit_llvm,
    link,
  };

  using Set = std::bitset<sizeof(Flags) * bits_per_byte>;

  Set set;

public:
  [[nodiscard]] auto EmitAssembly() const noexcept -> bool {
    return set[emit_assembly];
  }
  auto EmitAssembly(bool state) noexcept -> bool {
    return set[emit_assembly] = state;
  }

  [[nodiscard]] auto EmitObject() const noexcept -> bool {
    return set[emit_object];
  }
  auto EmitObject(bool state) noexcept -> bool {
    return set[emit_object] = state;
  }

  [[nodiscard]] auto EmitLLVM() const noexcept -> bool {
    return set[emit_llvm];
  }
  auto EmitLLVM(bool state) noexcept -> bool { return set[emit_llvm] = state; }

  [[nodiscard]] auto Link() const noexcept -> bool { return set[link]; }
  auto Link(bool state) noexcept -> bool { return set[link] = state; }
};

/**
 * @brief Holds all of the Options which the user can manipulate via the command
 * line.
 *
 */
class CLIOptions {
public:
  std::string             input_file;
  std::string             output_file;
  CLIFlags                flags;
  llvm::OptimizationLevel optimization_level;

  CLIOptions()
      : optimization_level(llvm::OptimizationLevel::O0) {}
  CLIOptions(std::string             infile,
             std::string             outfile,
             CLIFlags                flags,
             llvm::OptimizationLevel optimization_level)
      : input_file{std::move(infile)},
        output_file{std::move(outfile)},
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
  static auto RemoveTrailingExtensions(std::string filename) -> std::string;

  static auto PrintVersion(std::ostream &out) -> std::ostream &;
  static auto PrintHelp(std::ostream &out) -> std::ostream &;

  /**
   * @brief Get the exe file name
   *
   * @return std::string outfile + ""
   */
  [[nodiscard]] auto GetExeFilename() const -> std::string;

  /**
   * @brief Get the obj file name
   *
   * @return std::string outfile + ".o"
   */
  [[nodiscard]] auto GetObjFilename() const -> std::string;

  /**
   * @brief Get the asm file name
   *
   * @return std::string outfile + ".s"
   */
  [[nodiscard]] auto GetAsmFilename() const -> std::string;

  /**
   * @brief get the llvm file name
   *
   * @return std::string outfile + ".ll"
   */
  [[nodiscard]] auto GetLLVMFilename() const -> std::string;
};

auto ParseCLIOptions(std::ostream &out, int argc, char **argv) -> CLIOptions;

} // namespace pink
