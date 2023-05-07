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

#include <fstream>
#include <random>
#include <sstream>

#include "aux/Environment.h"

#include "ast/Function.h"
#include "ast/action/Codegen.h"

#include "ops/BinopPrimitives.h"
#include "ops/UnopPrimitives.h"

#include "support/FatalError.h"
#include "support/LLVMErrorToString.h"
#include "support/LLVMTypeToString.h"

#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"

#include "llvm/MC/TargetRegistry.h"

#include "llvm/IR/LegacyPassManager.h"

#include "llvm/Analysis/AliasAnalysis.h"

#include "llvm/Passes/PassBuilder.h"

namespace pink {
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *
 *                           Helper Functions
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
auto CompilationUnit::ToConstantInit(std::string_view text)
    -> std::vector<llvm::Constant *> {
  std::vector<llvm::Constant *> buffer{text.size()};
  auto                          cursor = buffer.begin();
  auto                          end    = buffer.end();
  for (auto character : text) {
    *cursor = ConstantCharacter(static_cast<uint8_t>(character));
    cursor++;

    if (cursor == end) {
      break;
    }
  }
  return buffer;
}

/*
 * #TODO: maybe we allow the caller to seed the rng?
 * or maybe there is an rng per Environment?
 * to be decided later.
 */
auto CompilationUnit::Gensym(std::string_view prefix) -> InternedString {
  std::random_device                 seed;
  std::mt19937                       generator{seed()};
  std::uniform_int_distribution<int> distribution;
  auto                               generate = [&]() {
    std::string sym{prefix};
    sym += std::to_string(distribution(generator));
    return sym;
  };
  const auto *candidate{variable_interner.Intern(generate())};
  while (scopes.LookupLocal(candidate).has_value()) {
    candidate = variable_interner.Intern(generate());
  }
  return candidate;
}

auto CompilationUnit::EmitFiles(std::ostream &err) const -> int {
  if (cli_options.DoEmitLLVMIR()) {
    if (EmitLLVMIRFile(err) == EXIT_FAILURE) {
      return EXIT_FAILURE;
    }
  }

  if (cli_options.DoEmitObject()) {
    if (EmitObjectFile(err) == EXIT_FAILURE) {
      return EXIT_FAILURE;
    }
  }

  if (cli_options.DoEmitAssembly()) {
    if (EmitAssemblyFile(err) == EXIT_FAILURE) {
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

auto CompilationUnit::EmitLLVMIRFile(std::ostream &err) const -> int {
  auto                 filename = cli_options.GetLLVMIRFile();
  std::error_code      outfile_error;
  llvm::raw_fd_ostream outfile{filename.c_str(), outfile_error};
  if (outfile_error) {
    err << "Couldn't open output file [" << filename << "] " << outfile_error
        << "\n";
    return EXIT_FAILURE;
  }

  module->print(outfile, nullptr);
  return EXIT_SUCCESS;
}

auto CompilationUnit::EmitObjectFile(std::ostream &err) const -> int {
  auto                 filename = cli_options.GetObjectFile();
  std::error_code      outfile_error{};
  llvm::raw_fd_ostream outfile{filename.c_str(), outfile_error};
  if (outfile_error) {
    err << "Couldn't open output file [" << filename << "] " << outfile_error
        << "\n";
    return EXIT_FAILURE;
  }

  llvm::legacy::PassManager AssemblyPrinter;

  bool failed{target_machine->addPassesToEmitFile(
      AssemblyPrinter,
      outfile,
      nullptr,
      llvm::CodeGenFileType::CGFT_ObjectFile)};

  if (failed) {
    err << "Cannot write an object file for Target Machine ["
        << target_machine->getTargetCPU().data() << ","
        << target_machine->getTargetTriple().str() << "]\n";
    return EXIT_FAILURE;
  }
  AssemblyPrinter.run(module.operator*());
  return EXIT_SUCCESS;
}

auto CompilationUnit::EmitAssemblyFile(std::ostream &err) const -> int {
  auto                 filename = cli_options.GetAssemblyFile();
  std::error_code      outfile_error{};
  llvm::raw_fd_ostream outfile{filename.c_str(), outfile_error};
  if (outfile_error) {
    err << "Couldn't open output file [" << filename << "] " << outfile_error
        << "\n";
    return EXIT_FAILURE;
  }

  llvm::legacy::PassManager AssemblyPrinter;

  bool failed{target_machine->addPassesToEmitFile(
      AssemblyPrinter,
      outfile,
      nullptr,
      llvm::CodeGenFileType::CGFT_AssemblyFile)};

  if (failed) {
    err << "Cannot write an assembly file for Target Machine ["
        << target_machine->getTargetCPU().data() << ","
        << target_machine->getTargetTriple().str() << "]\n";
    return EXIT_FAILURE;
  }
  AssemblyPrinter.run(module.operator*());
  return EXIT_SUCCESS;
}

auto CompilationUnit::Compile(std::ostream &out, std::ostream &err) -> int {
  if (DoVerbose()) {
    out << "Compiling source file [" << GetInputFile() << "]\n";
  }

  auto parse_result = ParseInputFile(err);
  if (!parse_result) {
    PrintErrorWithSourceText(err, parse_result.GetSecond());
    return EXIT_FAILURE;
  }

  auto typecheck_errors = TypecheckTerms(parse_result.GetFirst());
  if (typecheck_errors) {
    for (auto &error : typecheck_errors.value()) {
      PrintErrorWithSourceText(err, error);
    }
    return EXIT_FAILURE;
  }

  auto codegen_error = CodegenTerms(parse_result.GetFirst());
  if (codegen_error) {
    PrintErrorWithSourceText(err, codegen_error.value());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

auto CompilationUnit::ParseInputFile([[maybe_unused]] std::ostream &err)
    -> Outcome<Terms, Error> {
  Terms terms;
  {
    std::fstream infile;
    infile.open(GetInputFile());
    if (!infile.is_open()) {
      std::string errmsg{"Could not open input file ["};
      errmsg += GetInputFile();
      errmsg += "]\n";
      FatalError(errmsg);
    }

    SetIStream(&infile);
    while (!EndOfInput()) {
      auto term_result = Parse();

      if (!term_result) {
        auto &error = term_result.GetSecond();
        if ((error.code == Error::Code::EndOfFile) && (!terms.empty())) {
          break;
        }
        return std::move(error);
      }

      terms.emplace_back(std::move(term_result.GetFirst()));
    }
  }
  return terms;
}

auto CompilationUnit::TypecheckTerms(Terms &terms) -> std::optional<Errors> {
  Errors errors;
  for (const auto &term : terms) {
    auto typecheck_result = Typecheck(term, *this);

    if (!typecheck_result) {
      errors.emplace_back(std::move(typecheck_result.GetSecond()));
    }
  }

  if (errors.empty()) {
    return {};
  }

  return errors;
}

auto CompilationUnit::CodegenTerms(Terms &terms) -> std::optional<Error> {
  for (const auto &term : terms) {
    auto outcome = term->Codegen(*this);
    if (!outcome) {
      return std::move(outcome.GetSecond());
    }
  }
  return {};
}

// we may want to print errors at some point. (it happened for Compile and Link)
auto CompilationUnit::DefaultAnalysis([[maybe_unused]] std::ostream &err)
    -> int {
  // quote: buildPerModuleDefaultPipeline "... Level cannot be 'O0' here ..."
  if (GetOptimizationLevel() != llvm::OptimizationLevel::O0) {
    llvm::LoopAnalysisManager     LAM;
    llvm::FunctionAnalysisManager FAM;
    llvm::CGSCCAnalysisManager    CGAM;
    llvm::ModuleAnalysisManager   MAM;

    // https://llvm.org/doxygen/classllvm_1_1PassBuilder.html
    llvm::PassBuilder passBuilder;

    // #TODO: what are the default analysis that this constructs?
    FAM.registerPass([&] { return passBuilder.buildDefaultAAPipeline(); });

    // Register the AnalysisManagers with the Pass Builder
    passBuilder.registerModuleAnalyses(MAM);
    passBuilder.registerCGSCCAnalyses(CGAM);
    passBuilder.registerFunctionAnalyses(FAM);
    passBuilder.registerLoopAnalyses(LAM);
    // This registers each of the passes with eachother, so they
    // can perform optimizations together, lazily
    passBuilder.crossRegisterProxies(LAM, FAM, CGAM, MAM);

    llvm::ModulePassManager MPM =
        passBuilder.buildPerModuleDefaultPipeline(GetOptimizationLevel());

    // Run the optimizer against the IR
    MPM.run(*module, MAM);
  }
  return EXIT_SUCCESS;
}

auto CompilationUnit::NativeCPUFeatures() noexcept -> std::string {
  std::string           cpu_features;
  llvm::StringMap<bool> features;

  if (llvm::sys::getHostCPUFeatures(features)) {
    auto     idx      = features.begin();
    auto     end      = features.end();
    unsigned numElems = features.getNumItems();
    unsigned jdx      = 0;

    while (idx != end) {
      if ((*idx).getValue()) {
        cpu_features += std::string("+");
        cpu_features += idx->getKeyData();
      } else {
        cpu_features += std::string("-");
        cpu_features += idx->getKeyData();
      }

      if (jdx < (numElems - 1)) {
        cpu_features += ",";
      }

      idx++;
      jdx++;
    }
  }

  return cpu_features;
}

auto CompilationUnit::CreateNativeCompilationUnit(CLIOptions    cli_options,
                                                  std::istream *input)
    -> CompilationUnit {
  auto        context       = std::make_unique<llvm::LLVMContext>();
  std::string target_triple = llvm::sys::getProcessTriple();

  std::string         error;
  const llvm::Target *target =
      llvm::TargetRegistry::lookupTarget(target_triple, error);
  if (target == nullptr) {
    FatalError(error.data());
  }

  auto *target_machine =
      target->createTargetMachine(target_triple,
                                  llvm::sys::getHostCPUName().str(),
                                  NativeCPUFeatures(),
                                  llvm::TargetOptions{},
                                  llvm::Reloc::Model::PIC_,
                                  llvm::CodeModel::Model::Small);
  auto data_layout = target_machine->createDataLayout();

  auto instruction_builder = std::make_unique<llvm::IRBuilder<>>(*context);
  auto module =
      std::make_unique<llvm::Module>(cli_options.GetInputFile().stem().c_str(),
                                     *context);
  module->setSourceFileName(cli_options.GetInputFile().c_str());
  module->setDataLayout(data_layout);
  module->setTargetTriple(target_triple);

  CompilationUnit env{std::move(cli_options),
                      input,
                      std::move(context),
                      std::move(module),
                      std::move(instruction_builder),
                      target_machine};

  InitializeBinopPrimitives(env);
  InitializeUnopPrimitives(env);

  return env;
}

auto CompilationUnit::CreateTestCompilationUnit() -> CompilationUnit {
  CompilationUnit env;
  InitializeBinopPrimitives(env);
  InitializeUnopPrimitives(env);
  return env;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *
 *           LLVM IR generation (not the llvm::IRBuilder interface)
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/******************************* Allocation *******************************/
auto CompilationUnit::AllocateGlobalText(std::string_view name,
                                         std::string_view text)
    -> llvm::GlobalVariable * {
  // #TODO: ensure that globally allocated text is unique per module.
  // (and later between modules being combined) ((we can accomplish
  //  this with a llvm::StringMap<std::string_view>, where the value
  //  being looked up is the global strings name.))
  auto *type = LLVMTextType(text.size());
  auto *init = ConstantText(text);
  return AllocateGlobal(name, type, init);
}

auto CompilationUnit::AllocateGlobal(std::string_view name,
                                     llvm::Type      *type,
                                     llvm::Constant  *initializer)
    -> llvm::GlobalVariable * {
  auto *global =
      llvm::cast<llvm::GlobalVariable>(module->getOrInsertGlobal(name, type));
  if (initializer == nullptr) {
    return global;
  }
  global->setInitializer(initializer);
  return global;
}

// #TODO: we can emit lifetime information about
// alloca's in local scopes using
// llvm.lifetime.start(<size>, <ptr>)
// llvm.lifetime.end(<size>, <ptr>)
// this will allow the optimizer to reuse stack space.
// We can emit llvm.lifetime.start(<size>, <ptr>)
// in Environment::AllocateLocal, but we would need
// to delay emitting llvm.lifetime.end(<size>, <ptr>)
// until end of scope somehow. a delay primitive?
auto CompilationUnit::AllocateLocal(std::string_view name,
                                    llvm::Type      *type,
                                    llvm::Value *init) -> llvm::AllocaInst * {
  assert(current_function != nullptr);
  auto  save  = GetInsertionPoint();
  auto *entry = &current_function->getEntryBlock();
  instruction_builder->SetInsertPoint(entry, entry->begin());

  auto *alloca = CreateAlloca(type, nullptr, name);
  if (init != nullptr) {
    Store(type, init, alloca);
  }

  SetInsertionPoint(save);
  return alloca;
}

auto CompilationUnit::AllocateVariable(std::string_view name,
                                       llvm::Type      *type,
                                       llvm::Value     *init) -> llvm::Value * {
  if (current_function == nullptr) {
    if (auto *const_init = llvm::dyn_cast<llvm::Constant>(init)) {
      return AllocateGlobal(name, type, const_init);
    }
    FatalError("Non-Constant global initializer");
  }
  return AllocateLocal(name, type, init);
}

auto CompilationUnit::Load(llvm::Type *type, llvm::Value *source)
    -> llvm::Value * {
  // #RULE we can only load single value types
  // #RULE lhs of assignment suppresses a load
  // #RULE address of suppresses a load
  if (!type->isSingleValueType() || OnTheLHSOfAssignment() ||
      WithinAddressOf()) {
    return source;
  }

  return instruction_builder->CreateLoad(type, source);
}

void CompilationUnit::Store(llvm::Type  *type,
                            llvm::Value *source,
                            llvm::Value *destination) {
  // #RULE we can only store single value types
  if (type->isSingleValueType()) {
    instruction_builder->CreateStore(source, destination);
  } else {
    // #RULE store copies source into destination.
    StoreAggregate(type, source, destination);
  }
}

void CompilationUnit::StoreAggregate(llvm::Type  *type,
                                     llvm::Value *source,
                                     llvm::Value *destination) {
  if (auto *const_source = llvm::dyn_cast<llvm::Constant>(source);
      const_source != nullptr) {
    StoreConstAggregate(type, const_source, destination);
  } else {
    StoreValueAggregate(type, source, destination);
  }
}

void CompilationUnit::StoreConstAggregate(llvm::Type     *type,
                                          llvm::Constant *source,
                                          llvm::Value    *destination) {
  if (auto *array_type = llvm::dyn_cast<llvm::ArrayType>(type);
      array_type != nullptr) {
    auto       *element_type = array_type->getElementType();
    std::size_t size         = array_type->getNumElements();

    // if we are storing an array, we only have to check isSingleValueType once
    if (element_type->isSingleValueType()) {
      for (std::size_t index = 0; index < size; ++index) {
        auto *element_pointer =
            CreateConstInBoundsGEP2_64(array_type, destination, 0, index);
        auto *const_index = ConstantSize(index);
        instruction_builder->CreateStore(
            source->getAggregateElement(const_index),
            element_pointer);
      }
    } else {
      for (std::size_t index = 0; index < size; ++index) {
        auto *element_pointer =
            CreateConstInBoundsGEP2_64(array_type, destination, 0, index);
        auto *const_index = ConstantSize(index);
        StoreConstAggregate(element_type,
                            source->getAggregateElement(const_index),
                            element_pointer);
      }
    }
    return;
  }

  if (auto *struct_type = llvm::dyn_cast<llvm::StructType>(type);
      struct_type != nullptr) {
    unsigned size = struct_type->getNumElements();
    for (unsigned index = 0; index < size; ++index) {
      auto *const_init   = source->getAggregateElement(index);
      auto *element_type = const_init->getType();
      auto *element_pointer =
          CreateConstInBoundsGEP2_32(struct_type, destination, 0, index);
      // we are storing a struct, so we need to check isSingleValueType
      // for each element.
      if (const_init->getType()->isSingleValueType()) {
        instruction_builder->CreateStore(const_init, element_pointer);
      } else {
        StoreConstAggregate(element_type, const_init, element_pointer);
      }
    }
    return;
  }
  FatalError("unsupported type passed to StoreConstAggregate!");
}

void CompilationUnit::StoreValueAggregate(llvm::Type  *type,
                                          llvm::Value *source,
                                          llvm::Value *destination) {
  if (auto *array_type = llvm::dyn_cast<llvm::ArrayType>(type);
      array_type != nullptr) {
    std::size_t size         = array_type->getNumElements();
    auto       *element_type = array_type->getElementType();

    if (element_type->isSingleValueType()) {
      for (std::size_t index = 0; index < size; ++index) {
        auto *source_element =
            CreateConstInBoundsGEP2_64(array_type, source, 0, index);
        auto *destination_element =
            CreateConstInBoundsGEP2_64(array_type, destination, 0, index);
        auto *value =
            instruction_builder->CreateLoad(element_type, source_element);
        instruction_builder->CreateStore(value, destination_element);
      }
    } else {
      for (std::size_t index = 0; index < size; ++index) {
        auto *source_element =
            CreateConstInBoundsGEP2_64(array_type, source, 0, index);
        auto *destination_element =
            CreateConstInBoundsGEP2_64(array_type, destination, 0, index);
        StoreValueAggregate(element_type, source_element, destination_element);
      }
    }
    return;
  }

  if (auto *struct_type = llvm::dyn_cast<llvm::StructType>(type);
      struct_type != nullptr) {
    unsigned size = struct_type->getNumElements();

    for (unsigned index = 0; index < size; ++index) {
      auto *element_type = struct_type->getElementType(index);
      auto *source_element =
          CreateConstInBoundsGEP2_32(struct_type, source, 0, index);
      auto *destination_element =
          CreateConstInBoundsGEP2_32(struct_type, destination, 0, index);

      if (element_type->isSingleValueType()) {
        auto *value =
            instruction_builder->CreateLoad(element_type, source_element);
        instruction_builder->CreateStore(value, destination_element);
      } else {
        StoreValueAggregate(element_type, source_element, destination_element);
      }
    }
    return;
  }

  FatalError("unsupported type passed to StoreValueAggregate!");
}

/* "methods" for builtin types

  to explain the quotes:
  they are technically not even functions,
  and we are working with anonymous structs;
  but the analogy holds, as these are common
  procedures we need when working with the
  primitive struct types builtin to the langauge
  itself.
*/

/* Text */
auto CompilationUnit::LoadText(llvm::StructType *text_type,
                               llvm::Value      *text_ptr)
    -> std::pair<llvm::Value *, llvm::Value *> {
  return {LoadTextSize(text_type, text_ptr), TextPointer(text_type, text_ptr)};
}

auto CompilationUnit::LoadTextSize(llvm::StructType *text_type,
                                   llvm::Value *text_ptr) -> llvm::Value * {
  auto *size_type     = LLVMSizeType();
  auto *text_size_ptr = CreateConstInBoundsGEP2_32(text_type, text_ptr, 0, 0);
  return Load(size_type, text_size_ptr);
}

auto CompilationUnit::TextPointer(llvm::StructType *text_type,
                                  llvm::Value      *text_ptr) -> llvm::Value * {
  return CreateConstInBoundsGEP2_32(text_type, text_ptr, 0, 1);
}

void CompilationUnit::StoreText(llvm::StructType             *text_type,
                                llvm::Value                  *text_ptr,
                                llvm::ArrayRef<llvm::Value *> init) {
  StoreArray(text_type, text_ptr, init);
}

/* Slices */
auto CompilationUnit::LoadSlice(llvm::StructType *slice_type,
                                llvm::Value      *slice_ptr)
    -> std::tuple<llvm::Value *, llvm::Value *, llvm::Value *> {
  return {LoadSliceSize(slice_type, slice_ptr),
          LoadSliceOffset(slice_type, slice_ptr),
          LoadSlicePointer(slice_type, slice_ptr)};
}

auto CompilationUnit::LoadSliceSize(llvm::StructType *slice_type,
                                    llvm::Value *slice_ptr) -> llvm::Value * {
  return LoadStructElement(slice_type, slice_ptr, 0);
}

auto CompilationUnit::LoadSliceOffset(llvm::StructType *slice_type,
                                      llvm::Value *slice_ptr) -> llvm::Value * {
  return LoadStructElement(slice_type, slice_ptr, 1);
}

auto CompilationUnit::LoadSlicePointer(llvm::StructType *slice_type,
                                       llvm::Value      *slice_ptr)
    -> llvm::Value * {
  return LoadStructElement(slice_type, slice_ptr, 2);
}

void CompilationUnit::StoreSlice(llvm::StructType *slice_type,
                                 llvm::Value      *slice_ptr,
                                 llvm::Value      *size,
                                 llvm::Value      *offset,
                                 llvm::Value      *ptr) {
  StoreStructElement(slice_type, slice_ptr, size, 0);
  StoreStructElement(slice_type, slice_ptr, offset, 1);
  StoreStructElement(slice_type, slice_ptr, ptr, 2);
}

auto CompilationUnit::PtrToSliceElement(llvm::StructType *slice_type,
                                        llvm::Type       *element_type,
                                        llvm::Value      *slice_ptr,
                                        llvm::Value *index) -> llvm::Value * {
  auto [size, offset, ptr] = LoadSlice(slice_type, slice_ptr);
  BoundsCheck(size, offset, ptr);
  return CreateInBoundsGEP(element_type, ptr, {index});
}

auto CompilationUnit::SliceSubscript(llvm::StructType *slice_type,
                                     llvm::Type       *element_type,
                                     llvm::Value      *slice_ptr,
                                     llvm::Value      *index) -> llvm::Value * {

  if (OnTheLHSOfAssignment()) {
    OnTheLHSOfAssignment(false);
    auto result = PtrToSliceElement(slice_type, element_type, slice_ptr, index);
    OnTheLHSOfAssignment(true);
    return result;
  }

  if (WithinAddressOf()) {
    WithinAddressOf(false);
    auto result = PtrToSliceElement(slice_type, element_type, slice_ptr, index);
    WithinAddressOf(true);
    return result;
  }
  auto *element_ptr =
      PtrToSliceElement(slice_type, element_type, slice_ptr, index);
  return Load(element_type, element_ptr);
}

/* Arrays */
auto CompilationUnit::LoadArray(llvm::StructType *array_type,
                                llvm::Value      *array_ptr)
    -> std::pair<llvm::Value *, llvm::Value *> {
  return {LoadArraySize(array_type, array_ptr),
          ArrayBuffer(array_type, array_ptr)};
}

auto CompilationUnit::LoadArraySize(llvm::StructType *array_type,
                                    llvm::Value *array_ptr) -> llvm::Value * {
  return LoadStructElement(array_type, array_ptr, 0);
}

auto CompilationUnit::ArrayBuffer(llvm::StructType *array_type,
                                  llvm::Value *array_ptr) -> llvm::Value * {
  return PtrToStructElement(array_type, array_ptr, 1);
}

void CompilationUnit::StoreArray(llvm::StructType             *array_type,
                                 llvm::Value                  *array_ptr,
                                 llvm::ArrayRef<llvm::Value *> init) {
  auto *buffer_type  = array_type->getTypeAtIndex(1);
  auto  num_elements = buffer_type->getArrayNumElements();
  assert(init.size() <= num_elements);
  auto *element_type = buffer_type->getArrayElementType();
  auto *buffer       = ArrayBuffer(array_type, array_ptr);
  for (std::size_t index = 0; index < num_elements; ++index) {
    auto *element = CreateConstInBoundsGEP2_64(buffer_type, buffer, 0, index);
    Store(element_type, init[index], element);
  }
  StoreArraySize(array_type, array_ptr, ConstantSize(num_elements));
}

void CompilationUnit::StoreArraySize(llvm::StructType *array_type,
                                     llvm::Value      *array_ptr,
                                     llvm::Value      *size) {
  StoreStructElement(array_type, array_ptr, size, 0);
}

auto CompilationUnit::ArraySubscript(llvm::StructType *array_type,
                                     llvm::Value      *array_ptr,
                                     llvm::Value      *index) -> llvm::Value * {
  // #TODO remove InternalFlags, this is a hacky solution to the
  // problem of when to emit Loads/Stores.
  // It might be cleaner to always deal in pointers, and make code
  // that needs a value emit the load then and there, instead of
  // 'knowning' when to emit a load or not deep in nested code
  // that handles values.
  // as it stands know, essentially each use case of InternalFlags
  // is a workaround to a bug that gets created by misusing InternalFlags
  // in a location I didn't think it was necessary. The solution
  // is essentially virulent accross functions dealing with values.
  if (OnTheLHSOfAssignment()) {
    OnTheLHSOfAssignment(false);
    auto result = PtrToArrayElement(array_type, array_ptr, index);
    OnTheLHSOfAssignment(true);
    return result;
  }

  if (WithinAddressOf()) {
    WithinAddressOf(false);
    auto result = PtrToArrayElement(array_type, array_ptr, index);
    WithinAddressOf(true);
    return result;
  }

  return LoadArrayElement(array_type, array_ptr, index);
}

auto CompilationUnit::PtrToArrayElement(llvm::StructType *array_type,
                                        llvm::Value      *array_ptr,
                                        llvm::Value *index) -> llvm::Value * {
  auto *buffer_type   = array_type->getTypeAtIndex(1);
  auto [size, buffer] = LoadArray(array_type, array_ptr);
  BoundsCheck(size, index);
  return CreateInBoundsGEP(buffer_type, buffer, {ConstantSize(0), index});
}

auto CompilationUnit::UncheckedPtrToArrayElement(llvm::StructType *array_type,
                                                 llvm::Value      *array_ptr,
                                                 std::size_t       index)
    -> llvm::Value * {
  auto *buffer_type = array_type->getTypeAtIndex(1);
  auto *buffer      = ArrayBuffer(array_type, array_ptr);
  return CreateConstInBoundsGEP2_64(buffer_type, buffer, 0, index);
}

auto CompilationUnit::LoadArrayElement(llvm::StructType *array_type,
                                       llvm::Value      *array_ptr,
                                       llvm::Value *index) -> llvm::Value * {
  auto *buffer_type  = array_type->getTypeAtIndex(1);
  auto *element_type = buffer_type->getArrayElementType();
  auto *element_ptr  = PtrToArrayElement(array_type, array_ptr, index);
  return Load(element_type, element_ptr);
}

void CompilationUnit::StoreArrayElement(llvm::StructType *array_type,
                                        llvm::Value      *source,
                                        llvm::Value      *array_ptr,
                                        llvm::Value      *index) {
  auto *buffer_type  = array_type->getTypeAtIndex(1);
  auto *element_type = buffer_type->getArrayElementType();
  auto *element_ptr  = PtrToArrayElement(array_type, array_ptr, index);
  Store(element_type, source, element_ptr);
}

auto CompilationUnit::PtrToStructElement(llvm::StructType *struct_type,
                                         llvm::Value      *struct_ptr,
                                         unsigned index) -> llvm::Value * {
  assert(struct_type->indexValid(index));
  return CreateConstInBoundsGEP2_32(struct_type, struct_ptr, 0, index);
}

auto CompilationUnit::LoadStructElement(llvm::StructType *struct_type,
                                        llvm::Value      *struct_ptr,
                                        unsigned index) -> llvm::Value * {
  auto *element_ptr  = PtrToStructElement(struct_type, struct_ptr, index);
  auto *element_type = struct_type->getTypeAtIndex(index);
  return Load(element_type, element_ptr);
}

void CompilationUnit::StoreStructElement(llvm::StructType *struct_type,
                                         llvm::Value      *struct_ptr,
                                         llvm::Value      *source,
                                         unsigned          index) {
  auto *element_ptr  = PtrToStructElement(struct_type, struct_ptr, index);
  auto *element_type = struct_type->getTypeAtIndex(index);
  return Store(element_type, source, element_ptr);
}

/***************************** Error Handling *****************************/

void CompilationUnit::BoundsCheck(llvm::Value *upper, llvm::Value *index) {
  auto *lower = ConstantInteger(0);
  // if ((upper <= index) || (index < 0))
  //    RuntimeError()
  auto *over  = CreateICmpSLE(upper, index);
  auto *under = CreateICmpSGT(lower, index);
  auto *check = CreateOr(over, under);

  auto *out_bounds = CreateAndInsertBasicBlock("out_bounds");
  auto *in_bounds  = CreateBasicBlock("in_bounds");

  CreateCondBr(check, out_bounds, in_bounds);
  SetInsertionPoint(out_bounds);

  RuntimeError("index out of bounds", ConstantInteger(1));
  // #NOTE: this instruction is dead code, but is
  // necessary for the control flow graph of the
  // function.
  CreateBr(in_bounds);

  // set up calling code to insert code
  // after the branch.
  InsertBasicBlock(in_bounds);
  SetInsertionPoint(in_bounds);
}

void CompilationUnit::BoundsCheck(llvm::Value *upper,
                                  llvm::Value *offset,
                                  llvm::Value *index) {
  auto *new_index = CreateAdd(offset, index, "bounds_check");
  BoundsCheck(upper, new_index);
}

void CompilationUnit::RuntimeError(std::string_view description,
                                   llvm::Value     *exit_code) {
  auto *error_text = AllocateGlobalText(Gensym(), description);
  auto *text_type  = LLVMTextType(description.size());
  auto *sys_err    = ConstantInteger(2);
  SysWriteText(sys_err, text_type, error_text);
  SysExit(exit_code);
}

/***************************** System Calls *****************************/
auto CompilationUnit::SysWriteSlice(llvm::Value      *filenum,
                                    llvm::StructType *slice_type,
                                    llvm::Value      *slice_pointer)
    -> llvm::Value * {
  auto [size, offset, buffer] = LoadSlice(slice_type, slice_pointer);
  auto *length                = CreateSub(size, offset);
  return SysWrite(filenum, length, buffer);
}

auto CompilationUnit::SysWriteText(llvm::Value      *filenum,
                                   llvm::StructType *text_type,
                                   llvm::Value *text_pointer) -> llvm::Value * {
  auto [text_size, text_ptr] = LoadText(text_type, text_pointer);
  return SysWrite(filenum, text_size, text_ptr);
}

auto CompilationUnit::SysWrite(llvm::Value *filenum,
                               llvm::Value *size,
                               llvm::Value *buffer) -> llvm::Value * {
  auto *size_type    = LLVMSizeType();
  auto *pointer_type = LLVMPointerType();

  auto *mov_rdi_type = LLVMFunctionType(size_type, {size_type});
  auto *mov_rsi_type = LLVMFunctionType(pointer_type, {pointer_type});
  auto *mov_rdx_type = LLVMFunctionType(size_type, {size_type});
  auto *mov_rax_type = LLVMFunctionType(size_type, {});
  auto *syscall_type = LLVMFunctionType(size_type, {});

  auto *mov_rdi = [&]() {
    if (llvm::isa<llvm::ConstantInt>(filenum)) {
      return InlineAsm(mov_rdi_type, "mov rdi, $1", "={rdi},i");
    }
    return InlineAsm(mov_rdi_type, "mov rdi, $1", "={rdi},r");
  }();
  auto *mov_rsi = InlineAsm(mov_rsi_type, "mov rsi, $1", "={rsi},r");
  auto *mov_rdx = InlineAsm(mov_rdx_type, "mov rdx, $1", "={rdx},r");
  auto *mov_rax = InlineAsm(mov_rax_type, "mov rax, 1", "={rax}");
  auto *syscall = InlineAsm(syscall_type, "syscall", "={rax}");

  CreateCall(mov_rax, {});
  CreateCall(mov_rdi, {filenum});
  CreateCall(mov_rdx, {size});
  CreateCall(mov_rsi, {buffer});
  return CreateCall(syscall, {});
}

void CompilationUnit::SysExit(llvm::Value *exit_code) {
  auto *size_type    = LLVMSizeType();
  auto *void_type    = LLVMVoidType();
  auto *mov_rax_type = LLVMFunctionType(size_type, {});
  auto *mov_rdi_type = LLVMFunctionType(size_type, {size_type});
  auto *syscall_type = LLVMFunctionType(void_type, {});

  auto *mov_rdi = [&]() {
    if (llvm::isa<llvm::ConstantInt>(exit_code)) {
      return InlineAsm(mov_rdi_type, "mov rdi, $1", "={rdi},i");
    }
    return InlineAsm(mov_rdi_type, "mov rdi, $1", "={rdi},r");
  }();
  auto *mov_rax = InlineAsm(mov_rax_type, "mov rax, 60", "={rax}");
  auto *syscall = InlineAsm(syscall_type, "syscall", "");
  // we cast the exit code to a size_type (u64)
  auto *return_value = Cast(exit_code, size_type, true, true);

  CreateCall(mov_rdi, {return_value});
  CreateCall(mov_rax, {});
  CreateCall(syscall);
}

/***************************** Casting *****************************/
// #NOTE:
// Int -> Int     -: sext (lossless) | trunc (lossy)
// Int -> Uint    -: zext (lossy) | trunc (lossy)
// Int -> Float   -: sitofp (lossy)
// Int -> Ptr     -: inttoptr (lossless)
// Uint -> Uint   -: zext (lossless) | trunc (lossy)
// Uint -> Int    -: zext (lossless) | trunc (lossy)
// Uint -> Float  -: uitofp (lossy)
// Uint -> Ptr    -: inttoptr (lossless)
// Float -> Float -: fpext (lossless?) | fptrunc (lossy)
// Float -> Int   -: fptosi (lossy)
// Float -> Uint  -: fptoui (lossy)
// Ptr -> Ptr     -: bitcast (lossless)
// Ptr -> Int     -: ptrtoint (lossless)
// Ptr -> Uint    -: ptrtoint (lossless)
auto CompilationUnit::Cast(llvm::Value *source,
                           llvm::Type  *target_type,
                           bool         is_source_signed,
                           bool         is_target_signed) -> llvm::Value * {
  auto *source_type = source->getType();

  if (is_source_signed) {
    if (auto *from_type = llvm::dyn_cast<llvm::IntegerType>(source_type)) {
      return CastIntegerTo(source, from_type, target_type, is_target_signed);
    }
  }

  if (!is_source_signed) {
    if (auto *from_type = llvm::dyn_cast<llvm::IntegerType>(source_type)) {
      return CastUnsignedIntegerTo(source,
                                   from_type,
                                   target_type,
                                   is_target_signed);
    }
  }

  FatalError("unsupported type cast");
}

auto CompilationUnit::CastIntegerTo(llvm::Value       *source,
                                    llvm::IntegerType *from_type,
                                    llvm::Type        *target_type,
                                    bool is_target_signed) -> llvm::Value * {
  if (auto *to_type = llvm::dyn_cast<llvm::IntegerType>(target_type);
      to_type != nullptr) {
    if (is_target_signed) {
      return CastSExt(source, from_type, to_type);
    }
    return CastZExt(source, from_type, to_type);
  }

  FatalError("unsupported type cast");
}

auto CompilationUnit::CastUnsignedIntegerTo(
    llvm::Value          *source,
    llvm::IntegerType    *from_type,
    llvm::Type           *target_type,
    [[maybe_unused]] bool is_target_signed) -> llvm::Value * {
  if (auto *to_type = llvm::dyn_cast<llvm::IntegerType>(target_type);
      to_type != nullptr) {
    return CastZExt(source, from_type, to_type);
  }

  FatalError("unsupported type cast");
}

auto CompilationUnit::CastSExt(llvm::Value       *from,
                               llvm::IntegerType *from_type,
                               llvm::IntegerType *to_type) -> llvm::Value * {
  auto from_bitwidth = from_type->getBitWidth();
  auto to_bitwidth   = to_type->getBitWidth();

  if (from_bitwidth < to_bitwidth) {
    return instruction_builder->CreateSExt(from, to_type);
  }

  if (from_bitwidth > to_bitwidth) {
    return instruction_builder->CreateTrunc(from, to_type);
  }
  // from_bitwidth == to_bitwidth
  return instruction_builder->CreateBitCast(from, to_type);
}

auto CompilationUnit::CastZExt(llvm::Value       *from,
                               llvm::IntegerType *from_type,
                               llvm::IntegerType *to_type) -> llvm::Value * {
  auto from_bitwidth = from_type->getBitWidth();
  auto to_bitwidth   = to_type->getBitWidth();

  if (from_bitwidth < to_bitwidth) {
    return instruction_builder->CreateZExt(from, to_type);
  }

  if (from_bitwidth > to_bitwidth) {
    return instruction_builder->CreateTrunc(from, to_type);
  }
  // from_bitwidth == to_bitwidth
  return instruction_builder->CreateBitCast(from, to_type);
}

auto CompilationUnit::InlineAsm(llvm::FunctionType         *asm_type,
                                std::string_view            asm_string,
                                std::string_view            constraints,
                                bool                        has_side_effects,
                                bool                        is_align_stack,
                                llvm::InlineAsm::AsmDialect asm_dialect,
                                bool can_throw) -> llvm::InlineAsm * {
  if (auto error = llvm::InlineAsm::verify(asm_type, constraints)) {
    std::stringstream buffer;
    buffer << "InlineAsm constraints [" << constraints << "] are not valid "
           << "for llvm::Type [" << LLVMTypeToString(asm_type) << "] because ["
           << LLVMErrorToString(error) << "]\n";
    FatalError(buffer.str());
  }

  return llvm::InlineAsm::get(asm_type,
                              asm_string,
                              constraints,
                              has_side_effects,
                              is_align_stack,
                              asm_dialect,
                              can_throw);
}

void CompilationUnit::ConstructFunctionAttributes(
    llvm::Function             *llvm_function,
    pink::FunctionType::Pointer pink_function_type) {
  auto    *llvm_return_type   = llvm_function->getReturnType();
  auto    *llvm_function_type = llvm_function->getFunctionType();
  /*
  if this function returns a structure then we
  have already promoted it's return type to void,
  and added a parameter at the beginning of the
  argument list. This argument needs to be given
  the sret<Ty> attribute to communicate to llvm
  that this struct is the return value.
  this also means that the size of the argument list
  is off by one compared to the function type as in
  the source text. so we have to start counting
  the index from 1. additionally, any other non-SingleValueType
  will have been converted to an opaque pointer type,
  so we must use the arguments converted from the
  pink::FunctionType one by one, to add the byval<Ty>
  attributes to each of the opaque pointer type arguments.
  #NOTE: I would prefer to add parameter attributes
  when we convert the FunctionType within ToLLVM(),
  however the attributes themselves are associated
  with each function, as llvm::FunctionTypes may
  be shared among llvm::Functions. This means we
  are forced to walk the list of arguments types
  twice.

  actually, we might be able to accomplish this if
  we store the attributes computed for a given
  function_type in the type itself. how bad could
  that be?


  if the function returns a singleValueType the return
  value simply sits in the return register and we can
  start counting arguments from 0.
*/
  unsigned index = [&]() {
    if (llvm_return_type->isSingleValueType() &&
        !llvm_return_type->isVoidTy()) {
      llvm::AttrBuilder return_attribute(*context);
      return_attribute.addStructRetAttr(llvm_return_type);
      llvm_function->addParamAttrs(0U, return_attribute);
      return 1U;
    }
    return 0U;
  }();

  const auto &arguments            = pink_function_type->GetArguments();
  const auto  number_of_parameters = llvm_function_type->getNumParams();
  for (; index < number_of_parameters; ++index) {
    llvm::AttrBuilder parameter_attribute(*context);
    auto             *parameter_type = arguments[index]->ToLLVM(*this);

    if (!parameter_type->isSingleValueType() && !parameter_type->isVoidTy()) {
      parameter_attribute.addByValAttr(parameter_type);
      llvm_function->addParamAttrs(index, parameter_attribute);
    }
  }
}

void CompilationUnit::ConstructFunctionArguments(
    llvm::Function *llvm_function, pink::Function const *pink_function) {
  // Set up the arguments within the function's first BasicBlock,
  // A) Allocate all arguments
  // B) Initialize all arguments
  // we must do A for all arguments before we can do B for any
  // argument, because the llvm optimizer best handles all allocas
  // at the beginning of the functions first BasicBlock
  std::vector<llvm::AllocaInst *> arg_allocas;
  arg_allocas.reserve(pink_function->GetArguments().size());
  auto allocate_argument = [&](const llvm::Argument &arg) {
    arg_allocas.emplace_back(
        CreateAlloca(arg.getType(), nullptr, arg.getName()));
  };
  std::for_each(llvm_function->arg_begin(),
                llvm_function->arg_end(),
                allocate_argument);

  auto alloc_cursor    = arg_allocas.begin();
  auto alloc_end       = arg_allocas.end();
  auto pink_arg_cursor = pink_function->GetArguments().begin();
  auto pink_arg_end    = pink_function->GetArguments().end();
  for (auto &llvm_arg : llvm_function->args()) {
    auto       *alloc    = *alloc_cursor;
    const auto &pink_arg = *pink_arg_cursor;

    Store(llvm_arg.getType(), alloc, &llvm_arg);
    BindVariable(pink_arg.first, pink_arg.second, alloc);

    alloc_cursor++;
    pink_arg_cursor++;
    if (alloc_cursor == alloc_end || pink_arg_cursor == pink_arg_end) {
      break;
    }
  }
}

} // namespace pink
