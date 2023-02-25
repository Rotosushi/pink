#include <fstream> // std::fstream

#include "core/Compile.h"
#include "core/Link.h"
#include "core/Optimize.h"

#include "aux/Error.h" // pink::FatalError

#include "ast/action/Codegen.h"
#include "ast/action/Typecheck.h"

#include "llvm/Analysis/AliasAnalysis.h" // llvm::AAManager

#include "llvm/Passes/PassBuilder.h" // llvm::PassBuilder

#include "llvm/Support/TargetSelect.h"

/*
#NOTE: 2/12/2023
as of today the AddressSanitizer has detected memory leaks in our program.
luckily all of them are from memory only meant to be cleaned up right before
our process exits. Because according to the output of LeakSanitizer:
all of them appear to come from the allocations presumably from these
llvm::Initialize* functions. that is, the memory leaks are coming from llvm
global state objects, holding information about llvm::X86TargetMachine,
llvm::TargetMachine, createX86MCInstrInfo, llvm::DataLayout::parseSpecifier,
std::vector<llvm::MCCFIInstruction ...>, createX86MCRegisterInfo,
llvm::MCSubtargetInfo::InitMCProcessorInfo,
llvm::X86TargetMachine::X86TargetMachine(), llvm::DenseMap<llvm::MCRegister,
int, ...>, llvm::RegisterTargetMachine<llvm::X86TargetMachine>.
Which looks like the kind of global data that would be constructed to describe
the Native Assembly Target.
(which isn't going to be used unless/until we emit Assembly code.)

assumptions:
I assumed that llvm::InitLLVM cleaned up these objects on destruction,
as per the comment in:
_/llvm-project/llvm/include/Support/InitLLVM.h
"InitLLVM calls llvm_shutdown() on destruction, which cleans up
 ManagedStatic objects."

preliminary research:
_/llvm-project/llvm/lib/Support/InitLLVM.cpp
"InitLLVM::~InitLLVM() { llvm_shutdown(); }"

_/llvm-projects/llvm/lib/Support/ManagedStatic.cpp
"void llvm::llvm_shutdown() {
  while (StaticList)
    StaticList->destroy();
}"

and StaticList::destroy also looks like it works,
so clearly none of these parts are the problem.

Is there any other function which I am failing to call?

*/

namespace pink {
static auto CompileEnvironment(std::ostream &out,
                               std::ostream &err,
                               Environment  &env) -> int;

auto Compile(int argc, char **argv) -> int {

  auto &out         = std::cout;
  auto &err         = std::cerr;
  auto  cli_options = pink::ParseCLIOptions(err, argc, argv);
  auto  env         = pink::Environment::CreateNativeEnvironment(cli_options);

  if (CompileEnvironment(out, err, env) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }

  if ((env.GetOptimizationLevel() != llvm::OptimizationLevel::O0) &&
      (DefaultAnalysis(err, env) == EXIT_FAILURE)) {
    return EXIT_FAILURE;
  }

  if (env.EmitFiles(err) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }

  if (env.DoLink()) {
    return Link(out, err, env);
  }
  return EXIT_SUCCESS;
}

using Terms = std::vector<pink::Ast::Pointer>;

static auto ParseAndTypecheckInputFile(std::ostream &err, Environment &env)
    -> std::pair<Terms, bool> {
  Terms valid_terms;
  Terms invalid_terms;

  { // empty scope, to destroy infile after we are done using it.
    std::fstream infile;
    infile.open(env.GetInputFilename().data());
    if (!infile.is_open()) {
      err << "Could not open input file [" << env.GetInputFilename() << "]\n";
      return {};
    }
    env.SetIStream(&infile);

    while (!env.EndOfInput()) {
      auto term_result = env.Parse();
      if (!term_result) {
        auto &error = term_result.GetSecond();
        if ((error.code == Error::Code::EndOfFile) && (!valid_terms.empty())) {
          break; // out of while loop
        }
        env.PrintErrorWithSourceText(err, error);
        return {};
      }
      auto &term = term_result.GetFirst();

      auto type_result = Typecheck(term, env);
      if (!type_result) {
        auto &error = type_result.GetSecond();
        env.PrintErrorWithSourceText(err, error);
        invalid_terms.emplace_back(std::move(term));
      } else {
        valid_terms.emplace_back(std::move(term));
      }
    }

    // reset the scopes so that Codegen
    // doesn't redeclare variables when
    // it processes this env.
    // #NOTE: 2/23/23
    // This is a bit weird, and I am thinking
    // about a better way to handle this
    // issue of symbol redefinition between
    // Typechecking and Codegeneration.
    env.ResetScopes();
  }
  if (invalid_terms.empty()) {
    return {std::move(valid_terms), true};
  }
  return {std::move(invalid_terms), false};
}

static void CodegenValidTerms(const Terms &valid_terms, Environment &env) {
  for (const auto &term : valid_terms) {
    auto *value = Codegen(term, env);
    assert(value != nullptr);
  }
}

auto CompileEnvironment(std::ostream &out, std::ostream &err, Environment &env)
    -> int {
  if (env.DoVerbose()) {
    out << "Compiling source file [" << env.GetInputFilename() << "]\n";
  }

  auto terms = ParseAndTypecheckInputFile(err, env);
  if (!terms.second) {
    // we can do something with invalid terms here if we wanted/needed to
    return EXIT_FAILURE;
  }
  auto &valid_terms = terms.first;

  CodegenValidTerms(valid_terms, env);

  return EXIT_SUCCESS;
}
} // namespace pink
