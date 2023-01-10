

#include "aux/Environment.h"

#include "kernel/ops/BinopPrimitives.h"
#include "kernel/ops/UnopPrimitives.h"

#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"

#include "llvm/MC/TargetRegistry.h"

#include "llvm/Target/TargetMachine.h"

#include "llvm/Passes/OptimizationLevel.h"

namespace pink {
Environment::Environment(
    std::shared_ptr<TypecheckFlags> flags, std::shared_ptr<CLIOptions> options,
    std::shared_ptr<Parser> parser, std::shared_ptr<StringInterner> symbols,
    std::shared_ptr<StringInterner> operators,
    std::shared_ptr<TypeInterner> types, std::shared_ptr<SymbolTable> bindings,
    std::shared_ptr<BinopTable> binops, std::shared_ptr<UnopTable> unops,
    std::shared_ptr<llvm::LLVMContext> context,
    std::shared_ptr<llvm::Module> llvm_module,
    std::shared_ptr<llvm::IRBuilder<>> instruction_builder,
    // std::shared_ptr<llvm::DIBuilder>             debug_builder,
    const llvm::Target *target, llvm::TargetMachine *target_machine,
    const llvm::DataLayout &data_layout)
    : false_bindings(std::make_shared<std::vector<InternedString>>()),
      flags(std::move(flags)), options(std::move(options)),
      parser(std::move(parser)), symbols(std::move(symbols)),
      operators(std::move(operators)), types(std::move(types)),
      bindings(std::move(bindings)), binops(std::move(binops)),
      unops(std::move(unops)), context(std::move(context)),
      llvm_module(std::move(llvm_module)),
      instruction_builder(std::move(instruction_builder)),
      // debug_builder(debug_builder),
      target(target), target_machine(target_machine), data_layout(data_layout),
      current_function(nullptr) {}

Environment::Environment(const Environment &env,
                         std::shared_ptr<SymbolTable> bindings)
    : false_bindings(std::make_shared<std::vector<InternedString>>()),
      flags(env.flags), options(env.options), parser(env.parser),
      symbols(env.symbols), operators(env.operators), types(env.types),
      bindings(std::move(bindings)), binops(env.binops), unops(env.unops),
      context(env.context), llvm_module(env.llvm_module),
      instruction_builder(env.instruction_builder),
      // debug_builder(env.debug_builder),
      target(env.target), target_machine(env.target_machine),
      data_layout(env.data_layout), current_function(env.current_function) {}

Environment::Environment(const Environment &env,
                         std::shared_ptr<SymbolTable> bindings,
                         std::shared_ptr<llvm::IRBuilder<>> instruction_builder,
                         llvm::Function *current_function)
    : false_bindings(std::make_shared<std::vector<InternedString>>()),
      flags(env.flags), options(env.options), parser(env.parser),
      symbols(env.symbols), operators(env.operators), types(env.types),
      bindings(std::move(bindings)), binops(env.binops), unops(env.unops),
      context(env.context), llvm_module(env.llvm_module),
      instruction_builder(std::move(instruction_builder)),
      // debug_builder(env.debug_builder),
      target(env.target), target_machine(env.target_machine),
      data_layout(env.data_layout), current_function(current_function) {}

Environment::Environment(const Environment &env,
                         std::shared_ptr<llvm::IRBuilder<>> builder)
    : false_bindings(std::make_shared<std::vector<InternedString>>()),
      flags(env.flags), options(env.options), parser(env.parser),
      symbols(env.symbols), operators(env.operators), types(env.types),
      bindings(env.bindings), binops(env.binops), unops(env.unops),
      context(env.context), llvm_module(env.llvm_module),
      instruction_builder(std::move(builder)), target(env.target),
      target_machine(env.target_machine), data_layout(env.data_layout),
      current_function(env.current_function) {}

void Environment::PrintErrorWithSourceText(std::ostream &out,
                                           const Error &error) {
  auto bad_source = parser->ExtractLine(error.loc);
  error.Print(out, bad_source);
}

void Environment::ClearFalseBindings() noexcept {
  for (const auto *binding : *false_bindings) {
    bindings->Unbind(binding);
  }

  false_bindings->clear();
}

auto Environment::NewGlobalEnv(std::shared_ptr<CLIOptions> options)
    -> std::unique_ptr<Environment> {
  return NewGlobalEnv(std::move(options), &std::cin);
}

auto Environment::NewGlobalEnv(std::shared_ptr<CLIOptions> options,
                               std::istream *instream)
    -> std::unique_ptr<Environment> {
  auto flags = std::make_shared<TypecheckFlags>();
  auto parser = std::make_shared<Parser>(instream);
  auto symbols = std::make_shared<StringInterner>();
  auto operators = std::make_shared<StringInterner>();
  auto types = std::make_shared<TypeInterner>();
  auto bindings = std::make_shared<SymbolTable>();
  auto binops = std::make_shared<BinopTable>();
  auto unops = std::make_shared<UnopTable>();

  std::shared_ptr<llvm::LLVMContext> context =
      std::make_shared<llvm::LLVMContext>();

  // This works for native code generation.
  // because the compiler is a process running
  // on the host machine. so the processTriple would be a
  // target triple suitable to generating code for the host machine.
  // that is, native code generation.
  // then cross platform codegeneration would be specified via the
  // command line, and as such we can write a switch over known and
  // supported generation targets to select the correct target_triple.
  // (and cpu, cpu features, and any other target specific information.)
  std::string target_triple = llvm::sys::getProcessTriple();

  std::string error;
  const llvm::Target *target =
      llvm::TargetRegistry::lookupTarget(target_triple, error);

  if (target == nullptr) {
    FatalError(error.data(), __FILE__, __LINE__);
  }

  // get the native CPU name.
  std::string cpu = llvm::sys::getHostCPUName().str();

  // get the native CPU features.
  std::string cpu_features;
  llvm::StringMap<bool> features;

  if (llvm::sys::getHostCPUFeatures(features)) {
    auto idx = features.begin();
    auto end = features.end();
    unsigned numElems = features.getNumItems();
    unsigned jdx = 0;

    while (idx != end) {
      // the StringMap<bool> maps strings (keys) to bools (values)
      // where the bool is true if the feature string is available on the target
      // and the bool is false if the feature string is unavailable on the
      // target. each feature available (and specified within the feature
      // string) must be prefixed with '+', and similarly unavailable features
      // must be prefixed with '-'.
      if ((*idx).getValue()) {
        cpu_features += std::string("+") + (*idx).getKeyData();
      } else {
        cpu_features += std::string("-") + (*idx).getKeyData();
      }

      if (jdx < (numElems - 1)) { // comma separate the features
        cpu_features += ",";
      }

      idx++;
      jdx++;
    }
  }
  // #TODO: TargetOptions works fine default constructed, but
  // reading the docs, and the class definition, this
  // structure seems like it is used for
  // optimization/profiling and specifying extra rules
  // for compilation, like what floating point rules to
  // use, and such. This might be useful when considering
  // optimizations to turn on or specific optimization flags
  // available to the user. 1/9/2023
  llvm::TargetOptions target_options;
  // #TODO: position independant code is a fine default,
  // however we should allow the user to change this via the command line
  // 1/9/2023
  llvm::Reloc::Model code_relocation_model = llvm::Reloc::Model::PIC_;
  // #TODO: the Small x86-64 code model is a fine default for x86-64,
  // however this must change if the code being compiled becomes larger
  // than 2GB, or if the data within the code or being processed by the
  // code becomes larger than 2GB. So we should add an option on the
  // command line to select between models. 1/9/2023
  // NOTE: this is something to look at for the future, does the
  // codegeneration fail if we don't meet the uder 2GB requirement?
  // would our program be able to recover from such an error?
  llvm::CodeModel::Model code_model = llvm::CodeModel::Model::Small;

  llvm::TargetMachine *target_machine = nullptr;
  target_machine = target->createTargetMachine(
      target_triple, cpu, cpu_features, target_options, code_relocation_model,
      code_model);

  llvm::DataLayout data_layout = target_machine->createDataLayout();

  auto instruction_builder = std::make_shared<llvm::IRBuilder<>>(*context);
  auto llvm_module =
      std::make_shared<llvm::Module>(options->input_file, *context);

  llvm_module->setSourceFileName(options->input_file);
  llvm_module->setDataLayout(data_layout);
  llvm_module->setTargetTriple(target_triple);

  // #TODO: construct classes which are used to generate
  // debugging information for a given source unit.

  // a minor gripe about the DIBuilder class, the class itself takes
  // as an argument a pointer to a DICompileUnit, which makes complete
  // sense, to associate the Builder of debug info with a class
  // which holds data common to debugging information, the DICompileUnit.
  // the issue is that DIBuilder is also the class with a method to
  // construct a DICompileUnit from arguments. yet no way to set the constructed
  // compile unit as the current one associated with the class?
  // to me it seems that the most natural way of constructing these two classes
  // is to create the CompileUnit and then the DIBuilder, so then why
  // does DIBuilder hold a method for constructing a DICompileUnit?
  // Is the intended flow to construct a DIBuilder, use it to make a
  // DICompileUnit, then make another DIBuilder associated with the
  // DICompileUnit? It seems like there is a redundant step in there.
  //

  // okay, so a somewhat minor thing to note is that if this language can
  // actually support functions as values then we will run into issues telling
  // the debugger that our language conforms to the C calling convention.
  // Our functions are slightly different from the ccc, so how do we tell that 
  // to the debugger? (maybe we can get away with telling the debugger 
  // all arguments are accessed through a single struct parameter somehow?)

  // const llvm::DIBuilder temp_debug_builder(*llvm_module);
  // llvm::DICompileUnit* debug_compile_unit =
  // temp_debug_builder->createCompileUnit(llvm::dwarf::DW_LANG_C);

  // std::shared_ptr<llvm::DIBuilder> debug_builder =
  // std::make_shared<llvm::DIBuilder>(*llvm_module, /* allowUnresolved = */
  // true, debug_compile_unit);

  auto env = std::unique_ptr<Environment>(new Environment(
      flags, std::move(options), parser, symbols, operators, types, bindings,
      binops, unops, context, llvm_module, instruction_builder, target,
      target_machine, data_layout));

  InitializeBinopPrimitives(*env);
  InitializeUnopPrimitives(*env);

  return env;
}

auto Environment::NewLocalEnv(const Environment &outer,
                              std::shared_ptr<SymbolTable> bindings)
    -> std::unique_ptr<Environment> {
  return std::unique_ptr<Environment>(new Environment(outer, bindings));
}

auto Environment::NewLocalEnv(const Environment &outer,
                              std::shared_ptr<SymbolTable> bindings,
                              std::shared_ptr<llvm::IRBuilder<>> builder,
                              llvm::Function *function)
    -> std::unique_ptr<Environment> {
  return std::unique_ptr<Environment>(
      new Environment(outer, bindings, builder, function));
}

auto Environment::NewLocalEnv(const Environment &outer,
                              std::shared_ptr<llvm::IRBuilder<>> builder)
    -> std::unique_ptr<Environment> {
  return std::unique_ptr<Environment>(new Environment(outer, builder));
}
} // namespace pink
