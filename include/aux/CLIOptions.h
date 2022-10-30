/**
 * @file CLIOptions.h
 * @brief Header for class CLIOptions
 * @version 0.1
 *
 */
#pragma once
#include <iostream>
#include <memory>
#include <string>

#include "llvm/Passes/OptimizationLevel.h"
#include "llvm/Passes/PassBuilder.h" // llvm::PassBuilder

/*
        In order to handle program options like --version or -v, etc...

        The easiest thing to do would be to include "getopt.h"
        and use getopt_long(...) to parse the options provided
        to the program.

        This works fantastically on unix like operating systems, and
        this header file should be available on almost any conceivable
        POSIX operating system. Which shouldn't limit this programs
        distributability within the wide array of flavors of Linux.

        However, getopt.h is not available on Windows, and I have no idea
        of it's availability on Apple Computers. So if we want this
        compiler to be available cross platform, using getopt might not
        facilitate that directly. On windows in particular there are several
        workarounds, like MinGW/CygWin (whichever is still supported) or the
        Windows Subsystem for Linux, which runs a full linux OS within the
        host windows system. For apple devices it seems like there is a
        workaround with macports?
        https://stackoverflow.com/questions/402377/using-getopts-to-process-long-and-short-command-line-options
        This requires an additional dependency for installation on OSX computers
        however.

        So, since this is a small project of just me so far, releasing only
        on Linux to begin seems like a fine option.
        Thus, using getopt_long seems like a perfectly reasonable solution
        to this particular issue for the time being.
        Noting down that this might be a hiccup in the future also seems like
        a reasonable compromise solution to get something working and usable
        by others while	I am working by myself.

        And in that line of thinking, what are some good options to
   have/consider for a 'complete' compiler? Obviously the usual suspects of

        -h or --help, -v or --version, --verbose, -i or --input, -o or --output,
   etc...

        and it would also seem prudent to provide things that control
        how llvm is used by the program.

        --emit-llvm, --optimization-level ..., --use-linker ..., -Wall (or
   equivalent) etc..

        #TODO: add support for multiple input files
*/

namespace pink {
/**
 * @brief Holds all of the Options which the user can manipulate via the command
 * line.
 *
 */
class CLIOptions {
public:
  /**
   * @brief holds the name of the file to read from, as specified on the command
   * line
   *
   */
  std::string input_file;
  /**
   * @brief holds the name of the file to write to, as specified on the command
   * line, or as computed by from the input file specified and the
   * [emit_assembly](#CLIOptions::emit_assembly),
   * [emit_object](#CLIOptions::emit_object), and
   * [emit_llvm](#CLIOptions::emit_llvm) options.
   *
   * the default output file name is the input file name with any extensions
   * stripped off thus if the input file was: "my/directory/to/input_file.p" the
   * default output file name will be: "my/directory/to/input_file"
   *
   * the default object file name will be the default ouput file name with the
   * extension ".o" added. thus if the input file was the same as above the
   * default object file name would be: "my/directory/to/input_file.o"
   *
   * the default llvm file name will be the default output file name with the
   * extension of ".ll" added. thus if the input file was the same as above the
   * default llvm file name would be: "my/directory/to/input_file.ll"
   *
   * the default assembly file name will be the default output file name with
   * the extension of ".s" added. thus if the input file was the same as above
   * the default assembly file name would be: "my/directory/to/input_file.s"
   */
  std::string output_file;

  /**
   * @brief set the verbosity level of the program
   *
   * the program will be more explicit about what it is doing
   * when verbose is set to true. all verbose messages are written
   * to std::cerr
   *
   */
  bool verbose;

  /**
   * @brief set to emit an assembly file
   * 	defaults to false.
   * 	if set to true, disables emit_object, emit_llvm, and link
   */
  bool emit_assembly;

  /**
   * @brief set to emit an object file
   * defaults to true.
   * if set to true explicitly, disables emit_assembly, emit_llvm, and link
   */
  bool emit_object;

  /**
   * @brief set to emit an llvm file
   * defaults to false.
   * if set to true, disables emit_assembly, emit_object, and link
   */
  bool emit_llvm;

  /**
   * @brief set to link the emitted object file into an executable
   * the link step only occurs if emit_object is also true
   */
  bool link;

  /**
   * @brief set the optimization level of emitted code.
   *
   * setting this to anything other than O0 causes the compiler
   * to run an [optimization] [pipeline] upon the [llvm_module] before emitting
   * the output file.
   *
   * [optimization]: https://llvm.org/docs/Passes.html
   * [pipeline]: https://llvm.org/docs/NewPassManager.html
   * [llvm_module]: https://llvm.org/docs/classllvm_1_1Module.html
   */
  llvm::OptimizationLevel optimization_level;
  // std::string linker-name;
  // etc...

  /**
   * @brief Construct a new CLIOptions
   *
   */
  CLIOptions();

  /**
   * @brief Construct a new CLIOptions object
   *
   * @param infile the input file's name, this is passed directly to
   * std::fstream::open()
   * @param outfile the output file's name, this is passed directly to
   * std::fstream::open()
   * @param verbose the vebose level of this execution
   * @param emit_assembly whether or not to emit an assembly file
   * @param emit_object whether or not to emit an object file
   * @param emit_llvm whether or not to emit an llvm file
   * @param link whether or not to run the linker on an emitted object file.
   * @param optimization_level the optimization level of this execution
   *
   */
  CLIOptions(std::string infile, std::string outfile, bool verbose,
             bool emit_assembly, bool emit_object, bool emit_llvm, bool link,
             llvm::OptimizationLevel optimization_level);

  /**
   * @brief Remove trailing filename extensions
   *
   * searches for the first '.' appearing after the last '/' in
   * the given input string and returns the string containing
   * everything up to that point. if there is no '.' in the
   * input string, then the entire string is returned.
   *
   * @param filename the filename to remove the extensions of
   * @return std::string the new filename with no extensions
   */
  static auto RemoveTrailingExtensions(std::string filename) -> std::string;

  /**
   * @brief Print the version of pink
   *
   * @param out the output stream to write to
   * @return std::ostream& the given output stream
   */
  static auto PrintVersion(std::ostream &out) -> std::ostream &;

  /**
   * @brief Print the Help text
   *
   * @param out the output stream to write to
   * @return std::ostream& the given output stream
   */
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

/**
 * @brief Parses the command line options and constructs a new CLIOptions
 * holding the values parsed
 *
 * calls getopt_long
 *
 * @param out the output stream for any message that ParseCLIOptions will write.
 * @param argc the argc as received from main
 * @param argv the argv as received from main
 * @return std::shared_ptr<CLIOptions> A new instance of a CLIOptions, holding
 * the options parsed from the command line arguments
 */
auto ParseCLIOptions(std::ostream &out, int argc, char **argv)
    -> std::shared_ptr<CLIOptions>;

} // namespace pink
