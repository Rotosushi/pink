/**
 * @file Environment.h
 * @brief Header for class Environment
 * @version 0.1
 *
 */
#pragma once
// #include "llvm/IR/DIBuilder.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InlineAsm.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#include "llvm/Target/TargetMachine.h"

#include "aux/CLIOptions.h"
#include "aux/Error.h"
#include "aux/InternalFlags.h"
#include "aux/ScopeStack.h"
#include "aux/StringInterner.h"
#include "aux/TypeInterner.h"

#include "ops/BinopTable.h"
#include "ops/UnopTable.h"

#include "front/Parser.h"

namespace pink {

/**
 * @brief Environment owns all of the data structures which are shared between
 * the different algorithms within the compiler. Essentially the Environment
 * exists to keep the call signatures of these algorithms smaller, as the other
 * option is to pass the needed data structures in by reference anyways.
 * another benefiet is that it provides a convienent place for placing the
 * initialization work of the llvm data structures.
 *
 * #TODO: this class probably does too much, but how do we reduce it in size?
 *  we very much need all of these members working together to run the process
 *  of compilation.
 *
 * #TODO: Environment -> CompilationUnit (because thats what it is)
 */
class Environment {
private:
  EnvironmentFlags internal_flags;
  CLIOptions       cli_options;
  Parser           parser;
  StringInterner   variable_interner;
  StringInterner   operator_interner;
  TypeInterner     type_interner;
  ScopeStack       scopes;
  BinopTable       binop_table;
  UnopTable        unop_table;

  std::unique_ptr<llvm::LLVMContext> context;
  std::unique_ptr<llvm::Module>      module;
  std::unique_ptr<llvm::IRBuilder<>> instruction_builder;
  llvm::TargetMachine               *target_machine;
  llvm::Function                    *current_function;
  // 2/6/2023
  // we still are not ready to add debug information just
  // yet. even though we cannot implement functions as values
  // as we originally hoped. Debug information will be added
  // once the language itself is more nailed down in syntax
  // and semantics.
  // std::unique_ptr<llvm::DIBuilder>   debug_builder;

  Environment(CLIOptions                         cli_options,
              std::istream                      *input,
              std::unique_ptr<llvm::LLVMContext> context,
              std::unique_ptr<llvm::Module>      module,
              std::unique_ptr<llvm::IRBuilder<>> instruction_builder,
              llvm::TargetMachine               *target_machine)
      : internal_flags{},
        cli_options{std::move(cli_options)},
        parser{input},
        variable_interner{},
        operator_interner{},
        type_interner{},
        scopes{},
        binop_table{},
        unop_table{},
        context{std::move(context)},
        module{std::move(module)},
        instruction_builder{std::move(instruction_builder)},
        target_machine{target_machine},
        current_function{nullptr} {
    assert(input != nullptr);
    assert(target_machine != nullptr);
  }

  Environment()
      : internal_flags{},
        cli_options{},
        parser{},
        variable_interner{},
        operator_interner{},
        type_interner{},
        scopes{},
        binop_table{},
        unop_table{},
        context{nullptr},
        module{nullptr},
        instruction_builder{nullptr},
        target_machine{nullptr},
        current_function{nullptr} {}

public:
  ~Environment()                                            = default;
  Environment(const Environment &other)                     = delete;
  Environment(Environment &&other)                          = default;
  auto operator=(const Environment &other) -> Environment & = delete;
  auto operator=(Environment &&other) -> Environment      & = default;

  // Environment methods
  auto ToConstantInit(std::string_view text) -> std::vector<llvm::Constant *>;

  auto Gensym(std::string_view prefix = "__") -> InternedString;

  void PrintErrorWithSourceText(std::ostream &out, const Error &error) const {
    auto bad_source = parser.ExtractSourceLine(error.location);
    error.Print(out, bad_source);
  }

  auto EmitFiles(std::ostream &err) const -> int;
  auto EmitObjectFile(std::ostream &err) const -> int;
  auto EmitAssemblyFile(std::ostream &err) const -> int;

  static auto NativeCPUFeatures() noexcept -> std::string;
  static auto CreateNativeEnvironment(CLIOptions    cli_options,
                                      std::istream *input = &std::cin)
      -> Environment;
  static auto CreateNativeEnvironment() -> Environment {
    return CreateNativeEnvironment(CLIOptions{});
  }

  /**
   * @brief Create a Test Environment object
   *
   * \warning it is not safe to call any member function which
   * requires the llvm::* members to be active from a TestEnvironment.
   * this is a hack to track down a memory leak in the test rig.
   *
   * @return Environment
   */
  static auto CreateTestEnvironment() -> Environment;

  // exposing EnvironmentFlags interface
  [[nodiscard]] auto OnTheLHSOfAssignment() const -> bool {
    return internal_flags.OnTheLHSOfAssignment();
  }
  auto OnTheLHSOfAssignment(bool state) -> bool {
    return internal_flags.OnTheLHSOfAssignment(state);
  }

  [[nodiscard]] auto WithinAddressOf() const -> bool {
    return internal_flags.WithinAddressOf();
  }
  auto WithinAddressOf(bool state) -> bool {
    return internal_flags.WithinAddressOf(state);
  }

  [[nodiscard]] auto WithinDereferencePtr() const -> bool {
    return internal_flags.WithinDereferencePtr();
  }
  auto WithinDereferencePtr(bool state) -> bool {
    return internal_flags.WithinDereferencePtr(state);
  }

  [[nodiscard]] auto WithinBindExpression() const -> bool {
    return internal_flags.WithinBindExpression();
  }
  auto WithinBindExpression(bool state) -> bool {
    return internal_flags.WithinBindExpression(state);
  }

  // exposing CLIOptions interface
  [[nodiscard]] auto DoVerbose() const noexcept -> bool {
    return cli_options.DoVerbose();
  }
  [[nodiscard]] auto DoLink() const noexcept -> bool {
    return cli_options.DoLink();
  }
  [[nodiscard]] auto DoEmitObject() const noexcept -> bool {
    return cli_options.DoEmitObject();
  }
  [[nodiscard]] auto DoEmitAssembly() const noexcept -> bool {
    return cli_options.DoEmitAssembly();
  }

  [[nodiscard]] auto GetInputFilename() const -> std::string_view {
    return cli_options.GetInputFilename();
  }
  [[nodiscard]] auto GetExecutableFilename() const -> std::string_view {
    return cli_options.GetExecutableFilename();
  }
  [[nodiscard]] auto GetObjectFilename() const -> std::string_view {
    return cli_options.GetObjectFilename();
  }
  [[nodiscard]] auto GetAssemblyFilename() const -> std::string_view {
    return cli_options.GetAssemblyFilename();
  }

  [[nodiscard]] auto GetOptimizationLevel() const -> llvm::OptimizationLevel {
    return cli_options.GetOptimizationLevel();
  }

  // exposing Parser's interface
  [[nodiscard]] auto EndOfInput() const -> bool { return parser.EndOfInput(); }
  [[nodiscard]] auto ExtractSourceLine(const Location &location) const
      -> std::string_view {
    return parser.ExtractSourceLine(location);
  }
  [[nodiscard]] auto GetIStream() const -> std::istream * {
    return parser.GetIStream();
  }
  void SetIStream(std::istream *input_stream) {
    return parser.SetIStream(input_stream);
  }
  auto Parse() -> Parser::Result { return parser.Parse(*this); }

  // exposing variable_interner's interface
  auto InternVariable(std::string_view str) -> InternedString {
    return variable_interner.Intern(str);
  }

  // exposing operator_interner's interface
  auto InternOperator(std::string_view str) -> InternedString {
    return operator_interner.Intern(str);
  }

  // exposing TypeInterner's interface
  auto GetNilType() -> NilType::Pointer { return type_interner.GetNilType(); }
  auto GetBoolType() -> BooleanType::Pointer {
    return type_interner.GetBoolType();
  }
  auto GetIntType() -> IntegerType::Pointer {
    return type_interner.GetIntType();
  }
  auto GetCharacterType() -> CharacterType::Pointer {
    return type_interner.GetCharacterType();
  }
  auto GetVoidType() -> VoidType::Pointer {
    return type_interner.GetVoidType();
  }

  auto GetFunctionType(Type::Pointer                  ret_type,
                       FunctionType::Arguments const &arg_types)
      -> FunctionType::Pointer {
    return type_interner.GetFunctionType(ret_type, arg_types);
  }
  auto GetFunctionType(Type::Pointer             ret_type,
                       FunctionType::Arguments &&arg_types)
      -> FunctionType::Pointer {
    return type_interner.GetFunctionType(ret_type, std::move(arg_types));
  }

  auto GetPointerType(Type::Pointer pointee_type) -> PointerType::Pointer {
    return type_interner.GetPointerType(pointee_type);
  }
  auto GetSliceType(Type::Pointer pointee_type) -> SliceType::Pointer {
    return type_interner.GetSliceType(pointee_type);
  }

  auto GetArrayType(std::size_t size, Type::Pointer element_type)
      -> ArrayType::Pointer {
    return type_interner.GetArrayType(size, element_type);
  }

  auto GetTupleType(TupleType::Elements const &elements) -> TupleType::Pointer {
    return type_interner.GetTupleType(elements);
  }
  auto GetTupleType(TupleType::Elements &&elements) -> TupleType::Pointer {
    return type_interner.GetTupleType(std::move(elements));
  }

  auto GetTextType(std::size_t length) -> ArrayType::Pointer {
    return type_interner.GetTextType(length);
  }

  // exposing ScopeStack's interface
  void ResetScopes() { scopes.Reset(); }
  void PushScope() { scopes.PushScope(); }
  void PopScope() { scopes.PopScope(); }

  auto LookupVariable(InternedString symbol)
      -> std::optional<ScopeStack::Symbol> {
    return scopes.Lookup(symbol);
  }
  auto LookupLocalVariable(InternedString symbol)
      -> std::optional<ScopeStack::Symbol> {
    return scopes.LookupLocal(symbol);
  }
  void
  BindVariable(InternedString symbol, Type::Pointer type, llvm::Value *value) {
    scopes.Bind(symbol, type, value);
  }

  // exposing BinopTable's interface
  auto RegisterBinop(InternedString         opr,
                     Precedence             precedence,
                     Associativity          associativity,
                     Type::Pointer          left_t,
                     Type::Pointer          right_t,
                     Type::Pointer          ret_t,
                     BinopCodegen::Function fn_p) -> BinopTable::Binop {
    return binop_table
        .Register(opr, precedence, associativity, left_t, right_t, ret_t, fn_p);
  }

  auto LookupBinop(InternedString opr) -> std::optional<BinopTable::Binop> {
    return binop_table.Lookup(opr);
  }

  // exposing UnopTable's interface
  auto RegisterUnop(InternedString        unop,
                    Type::Pointer         argument_type,
                    Type::Pointer         return_type,
                    UnopCodegen::Function generator) -> UnopTable::Unop {
    return unop_table.Register(unop, argument_type, return_type, generator);
  }

  auto LookupUnop(InternedString opr) -> std::optional<UnopTable::Unop> {
    return unop_table.Lookup(opr);
  }

  /*
   * LLVM IR generation (language 'primitives')
   */
  // Allocations
  auto AllocateGlobalText(std::string_view name, std::string_view text)
      -> llvm::GlobalVariable *;

  auto AllocateGlobal(std::string_view name,
                      llvm::Type      *type,
                      llvm::Constant  *initializer = nullptr)
      -> llvm::GlobalVariable *;

  auto AllocateLocal(std::string_view name,
                     llvm::Type      *type,
                     llvm::Value     *init = nullptr) -> llvm::AllocaInst *;

  auto AllocateVariable(std::string_view name,
                        llvm::Type      *type,
                        llvm::Value     *init = nullptr) -> llvm::Value *;

  // Loads and Stores
  auto Load(llvm::Type *type, llvm::Value *source) -> llvm::Value *;
  void Store(llvm::Type *type, llvm::Value *source, llvm::Value *destination);
  void StoreAggregate(llvm::Type  *type,
                      llvm::Value *source,
                      llvm::Value *destination);
  void StoreConstAggregate(llvm::Type     *type,
                           llvm::Constant *source,
                           llvm::Value    *destination);
  void StoreValueAggregate(llvm::Type  *type,
                           llvm::Value *source,
                           llvm::Value *destination);

  // "methods" for builtin types
  // #TODO: Store* "methods"
  auto LoadText(llvm::StructType *text_type, llvm::Value *text_ptr)
      -> std::pair<llvm::Value *, llvm::Value *>;
  auto LoadTextSize(llvm::StructType *text_type, llvm::Value *text_ptr)
      -> llvm::Value *;
  auto TextPointer(llvm::StructType *text_type, llvm::Value *text_ptr)
      -> llvm::Value *;
  auto StoreText(llvm::StructType             *text_type,
                 llvm::Value                  *text_ptr,
                 llvm::ArrayRef<llvm::Value *> init);

  // A slice could be two pointers, size would be equal to
  // the difference divided by the element type. But then
  // how would you walk backwards, you still need the size
  // of the buffer. offset = size-of-buffer - (index / element_size)
  // which is still three elements in a bidirectional slice.
  auto LoadSlice(llvm::StructType *slice_type, llvm::Value *slice_ptr)
      -> std::tuple<llvm::Value *, llvm::Value *, llvm::Value *>;
  auto LoadSlicePointer(llvm::StructType *slice_type, llvm::Value *slice_ptr)
      -> llvm::Value *;
  auto LoadSliceSize(llvm::StructType *slice_type, llvm::Value *slice_ptr)
      -> llvm::Value *;
  auto LoadSliceOffset(llvm::StructType *slice_type, llvm::Value *slice_ptr)
      -> llvm::Value *;
  void StoreSlice(llvm::StructType *slice_type,
                  llvm::Value      *slice_ptr,
                  llvm::Value      *size,
                  llvm::Value      *offset,
                  llvm::Value      *ptr);
  auto SliceSubscript(llvm::StructType *slice_type,
                      llvm::Type       *element_type,
                      llvm::Value      *slice_ptr,
                      llvm::Value      *index) -> llvm::Value *;

  auto LoadArray(llvm::StructType *array_type, llvm::Value *array_ptr)
      -> std::pair<llvm::Value *, llvm::Value *>;
  auto LoadArraySize(llvm::StructType *array_type, llvm::Value *array_ptr)
      -> llvm::Value *;
  auto ArrayBuffer(llvm::StructType *array_type, llvm::Value *array_ptr)
      -> llvm::Value *;
  void StoreArray(llvm::StructType             *array_type,
                  llvm::Value                  *array_ptr,
                  llvm::ArrayRef<llvm::Value *> init);
  auto ArraySubscript(llvm::StructType *array_type,
                      llvm::Value      *array_ptr,
                      llvm::Value      *index) -> llvm::Value *;

  auto PtrToSliceElement(llvm::StructType *slice_type,
                         llvm::Type       *element_type,
                         llvm::Value      *slice_ptr,
                         llvm::Value      *index) -> llvm::Value *;

  auto PtrToArrayElement(llvm::StructType *array_type,
                         llvm::Value      *ptr,
                         llvm::Value      *index) -> llvm::Value *;
  auto LoadArrayElement(llvm::StructType *array_type,
                        llvm::Value      *ptr,
                        llvm::Value      *index) -> llvm::Value *;
  void StoreArrayElement(llvm::StructType *array_type,
                         llvm::Value      *source,
                         llvm::Value      *ptr,
                         llvm::Value      *index);

  auto PtrToStructElement(llvm::StructType *struct_type,
                          llvm::Value      *ptr,
                          unsigned          index) -> llvm::Value *;
  auto LoadStructElement(llvm::StructType *struct_type,
                         llvm::Value      *ptr,
                         unsigned          index) -> llvm::Value *;
  void StoreStructElement(llvm::StructType *struct_type,
                          llvm::Value      *value,
                          llvm::Value      *ptr,
                          unsigned          index);
  // Casting
  auto Cast(llvm::Value *source,
            llvm::Type  *target_type,
            bool         is_source_signed,
            bool         is_target_signed) -> llvm::Value *;
  auto CastIntegerTo(llvm::Value       *source,
                     llvm::IntegerType *from_type,
                     llvm::Type        *to_type,
                     bool               is_target_signed) -> llvm::Value *;
  auto CastUnsignedIntegerTo(llvm::Value       *source,
                             llvm::IntegerType *from_type,
                             llvm::Type        *to_type,
                             bool is_target_signed) -> llvm::Value *;
  auto CastSExt(llvm::Value       *from,
                llvm::IntegerType *from_type,
                llvm::IntegerType *to_type) -> llvm::Value *;
  auto CastZExt(llvm::Value       *from,
                llvm::IntegerType *from_type,
                llvm::IntegerType *to_type) -> llvm::Value *;

  // Error Handling
  void BoundsCheck(llvm::Value *upper, llvm::Value *index);
  void BoundsCheck(llvm::Value *upper, llvm::Value *offset, llvm::Value *index);
  void RuntimeError(std::string_view description, llvm::Value *exit_code);

  // System Calls
  auto SysWriteSlice(llvm::Value      *filedes,
                     llvm::StructType *slice_type,
                     llvm::Value      *slice_pointer) -> llvm::Value *;
  auto SysWriteText(llvm::Value      *filedes,
                    llvm::StructType *text_type,
                    llvm::Value      *text_pointer) -> llvm::Value *;
  auto SysWrite(llvm::Value *filedes, llvm::Value *size, llvm::Value *buffer)
      -> llvm::Value *;

  void SysExit(llvm::Value *exit_code);

  /*
   * Optimization
   */
  auto DefaultAnalysis(std::ostream &err) -> int;

  /*
   * llvm::Module interface
   */
  auto AllocaAddressSpace() -> unsigned {
    return module->getDataLayout().getAllocaAddrSpace();
  }

  /*
   * exposing llvm::IRBuilder interface
   */
  // llvm::Constant getters
  auto ConstantInteger(std::size_t value) -> llvm::ConstantInt * {
    return instruction_builder->getInt64(value);
  }

  auto ConstantSize(std::size_t value) -> llvm::ConstantInt * {
    return instruction_builder->getInt64(value);
  }

  auto ConstantCharacter(uint8_t character) -> llvm::ConstantInt * {
    return instruction_builder->getInt8(character);
  }

  auto ConstantBoolean(bool value) -> llvm::ConstantInt * {
    return instruction_builder->getInt1(value);
  }

  auto ConstantStruct(llvm::ArrayRef<llvm::Constant *> elements) {
    assert(!elements.empty());
    auto *type = llvm::ConstantStruct::getTypeForElements(elements);
    return llvm::ConstantStruct::get(type, elements);
  }

  auto ConstantBareArray(llvm::ArrayRef<llvm::Constant *> elements) {
    auto *element_type = elements.front()->getType();
    auto *type         = LLVMBareArrayType(element_type, elements.size());
    return llvm::ConstantArray::get(type, elements);
  }

  auto ConstantArray(llvm::ArrayRef<llvm::Constant *> elements) {
    auto *size = ConstantSize(elements.size());
    return ConstantStruct({size, ConstantBareArray(elements)});
  }

  auto ConstantText(std::string_view text) -> llvm::Constant * {
    return ConstantArray(ToConstantInit(text));
  }

  auto
  InlineAsm(llvm::FunctionType         *asm_type,
            std::string_view            asm_string,
            std::string_view            constraints,
            bool                        has_side_effects = true,
            bool                        is_align_stack   = false,
            llvm::InlineAsm::AsmDialect asm_dialect = llvm::InlineAsm::AD_Intel,
            bool                        can_throw = false) -> llvm::InlineAsm *;

  /*
   * llvm::Type* getters
   */
  auto LLVMIntegerType() -> llvm::IntegerType * {
    return instruction_builder->getInt64Ty();
  }

  auto LLVMSizeType() -> llvm::IntegerType * {
    return instruction_builder->getInt64Ty();
  }

  auto LLVMCharacterType() -> llvm::IntegerType * {
    return instruction_builder->getInt8Ty();
  }

  auto LLVMBooleanType() -> llvm::IntegerType * {
    return instruction_builder->getInt1Ty();
  }

  auto LLVMNilType() -> llvm::IntegerType * {
    return instruction_builder->getInt1Ty();
  }

  auto LLVMVoidType() -> llvm::Type * {
    return instruction_builder->getVoidTy();
  }

  auto LLVMPointerType(unsigned int address_space = 0U) -> llvm::PointerType * {
    return instruction_builder->getPtrTy(address_space);
  }

  auto LLVMSliceType(unsigned int address_space = 0U) -> llvm::StructType * {
    auto *size_type    = LLVMSizeType();
    auto *pointer_type = LLVMPointerType(address_space);
    return llvm::StructType::get(*context,
                                 {size_type, size_type, pointer_type});
  }

  auto LLVMStructType(llvm::ArrayRef<llvm::Type *> element_types)
      -> llvm::StructType * {
    return llvm::StructType::get(*context, element_types);
  }

  auto LLVMArrayType(llvm::Type *element_type, std::size_t size)
      -> llvm::StructType * {
    auto *size_type  = LLVMSizeType();
    auto *array_type = llvm::ArrayType::get(element_type, size);
    return llvm::StructType::get(*context, {size_type, array_type});
  }

  static auto LLVMBareArrayType(llvm::Type *element_type, std::size_t size)
      -> llvm::ArrayType * {
    return llvm::ArrayType::get(element_type, size);
  }

  auto LLVMTextType(std::size_t size) -> llvm::StructType * {
    return LLVMArrayType(LLVMCharacterType(), size);
  }

  static auto LLVMFunctionType(llvm::Type                  *return_type,
                               llvm::ArrayRef<llvm::Type *> argument_types,
                               bool is_vararg = false) -> llvm::FunctionType * {
    return llvm::FunctionType::get(return_type, argument_types, is_vararg);
  }

  // BasicBlock and InsertionPoint
  class InsertionPoint {
  public:
    llvm::BasicBlock          *block;
    llvm::BasicBlock::iterator point;
  };

  auto GetInsertionPoint() -> InsertionPoint {
    return {instruction_builder->GetInsertBlock(),
            instruction_builder->GetInsertPoint()};
  }

  void SetInsertionPoint(InsertionPoint point) {
    instruction_builder->SetInsertPoint(point.block, point.point);
  }

  void SetInsertionPoint(llvm::BasicBlock *block) {
    instruction_builder->SetInsertPoint(block);
  }

  auto CreateAndInsertBasicBlock(const llvm::Twine &name          = "",
                                 llvm::BasicBlock  *insert_before = nullptr)
      -> llvm::BasicBlock * {
    assert(current_function != nullptr);
    return llvm::BasicBlock::Create(*context,
                                    name,
                                    current_function,
                                    insert_before);
  }

  auto CreateBasicBlock(const llvm::Twine &name          = "",
                        llvm::BasicBlock  *insert_before = nullptr)
      -> llvm::BasicBlock * {
    return llvm::BasicBlock::Create(*context, name, nullptr, insert_before);
  }

  void InsertBasicBlock(llvm::BasicBlock *block) {
    assert(current_function != nullptr);
    current_function->insert(current_function->end(), block);
  }

  // llvm::Function helpers
  auto CreateFunction(llvm::FunctionType             *type,
                      llvm::GlobalValue::LinkageTypes linkage,
                      const llvm::Twine &name) -> llvm::Function * {
    auto *function = llvm::Function::Create(type, linkage, name, *module);
    assert(current_function == nullptr);
    current_function = function;
    return function;
  }

  void
  ConstructFunctionAttributes(llvm::Function             *llvm_function,
                              pink::FunctionType::Pointer pink_function_type);

  void ConstructFunctionArguments(llvm::Function       *llvm_function,
                                  pink::Function const *pink_function);

  void LeaveCurrentFunction() {
    current_function = nullptr;
    instruction_builder->ClearInsertionPoint();
  }

  // Create*
  auto CreateCall(llvm::FunctionCallee          callee,
                  llvm::ArrayRef<llvm::Value *> args = std::nullopt,
                  const llvm::Twine            &name = "",
                  llvm::MDNode *fp_math_tag = nullptr) -> llvm::CallInst * {
    return instruction_builder->CreateCall(callee, args, name, fp_math_tag);
  }

  auto CreateAlloca(llvm::Type        *type,
                    llvm::Value       *array_size = nullptr,
                    const llvm::Twine &name       = "") -> llvm::AllocaInst * {
    return instruction_builder->CreateAlloca(
        type,
        module->getDataLayout().getAllocaAddrSpace(),
        array_size,
        name);
  }

  auto CreateGEP(llvm::Type                   *type,
                 llvm::Value                  *pointer,
                 llvm::ArrayRef<llvm::Value *> indecies,
                 const llvm::Twine            &name = "") -> llvm::Value * {
    return instruction_builder->CreateGEP(type, pointer, indecies, name);
  }

  auto CreateInBoundsGEP(llvm::Type                   *type,
                         llvm::Value                  *pointer,
                         llvm::ArrayRef<llvm::Value *> indecies,
                         const llvm::Twine &name = "") -> llvm::Value * {
    return instruction_builder->CreateInBoundsGEP(type,
                                                  pointer,
                                                  indecies,
                                                  name);
  }

  auto CreateConstInBoundsGEP2_32(llvm::Type        *type,
                                  llvm::Value       *pointer,
                                  unsigned int       index0,
                                  unsigned int       index1,
                                  const llvm::Twine &name = "")
      -> llvm::Value * {
    return instruction_builder->CreateConstInBoundsGEP2_32(type,
                                                           pointer,
                                                           index0,
                                                           index1,
                                                           name);
  }

  auto CreateConstInBoundsGEP2_64(llvm::Type        *type,
                                  llvm::Value       *pointer,
                                  uint64_t           index0,
                                  uint64_t           index1,
                                  const llvm::Twine &name = "")
      -> llvm::Value * {
    return instruction_builder->CreateConstInBoundsGEP2_64(type,
                                                           pointer,
                                                           index0,
                                                           index1,
                                                           name);
  }

  auto CreateLoad(llvm::Type        *type,
                  llvm::Value       *address,
                  const llvm::Twine &name = "") {
    return instruction_builder->CreateLoad(type, address, name);
  }

  auto CreateStore(llvm::Value *value,
                   llvm::Value *address,
                   bool         is_volatile = false) {
    return instruction_builder->CreateStore(value, address, is_volatile);
  }

  auto CreateCondBr(llvm::Value      *test,
                    llvm::BasicBlock *true_branch,
                    llvm::BasicBlock *false_branch,
                    llvm::MDNode     *branch_weights = nullptr,
                    llvm::MDNode     *unpredictable  = nullptr)
      -> llvm::BranchInst * {
    return instruction_builder->CreateCondBr(test,
                                             true_branch,
                                             false_branch,
                                             branch_weights,
                                             unpredictable);
  }

  auto CreateBr(llvm::BasicBlock *block) -> llvm::BranchInst * {
    return instruction_builder->CreateBr(block);
  }

  auto CreatePHI(llvm::Type        *type,
                 unsigned int       reserved_values,
                 const llvm::Twine &name = "") -> llvm::PHINode * {
    return instruction_builder->CreatePHI(type, reserved_values, name);
  }

  auto CreateRetVoid() -> llvm::ReturnInst * {
    return instruction_builder->CreateRetVoid();
  }

  auto CreateRet(llvm::Value *return_value) -> llvm::ReturnInst * {
    return instruction_builder->CreateRet(return_value);
  }

  auto CreateAdd(llvm::Value       *left,
                 llvm::Value       *right,
                 const llvm::Twine &name             = "",
                 bool               no_unsigned_wrap = false,
                 bool               no_signed_wrap   = false) {
    return instruction_builder->CreateAdd(left,
                                          right,
                                          name,
                                          no_unsigned_wrap,
                                          no_signed_wrap);
  }

  auto CreateSub(llvm::Value       *left,
                 llvm::Value       *right,
                 const llvm::Twine &name             = "",
                 bool               no_unsigned_wrap = false,
                 bool               no_signed_wrap   = false) {
    return instruction_builder->CreateSub(left,
                                          right,
                                          name,
                                          no_unsigned_wrap,
                                          no_signed_wrap);
  }

  auto CreateMul(llvm::Value       *left,
                 llvm::Value       *right,
                 const llvm::Twine &name             = "",
                 bool               no_unsigned_wrap = false,
                 bool               no_signed_wrap   = false) {
    return instruction_builder->CreateMul(left,
                                          right,
                                          name,
                                          no_unsigned_wrap,
                                          no_signed_wrap);
  }

  auto CreateSDiv(llvm::Value       *left,
                  llvm::Value       *right,
                  const llvm::Twine &name     = "",
                  bool               is_exact = false) {
    return instruction_builder->CreateSDiv(left, right, name, is_exact);
  }

  auto CreateSRem(llvm::Value       *left,
                  llvm::Value       *right,
                  const llvm::Twine &name = "") {
    return instruction_builder->CreateSRem(left, right, name);
  }

  auto CreateICmpEQ(llvm::Value       *left,
                    llvm::Value       *right,
                    const llvm::Twine &name = "") {
    return instruction_builder->CreateICmpEQ(left, right, name);
  }

  auto CreateICmpNE(llvm::Value       *left,
                    llvm::Value       *right,
                    const llvm::Twine &name = "") {
    return instruction_builder->CreateICmpNE(left, right, name);
  }

  auto CreateICmpSGT(llvm::Value       *left,
                     llvm::Value       *right,
                     const llvm::Twine &name = "") {
    return instruction_builder->CreateICmpSGT(left, right, name);
  }

  auto CreateICmpSGE(llvm::Value       *left,
                     llvm::Value       *right,
                     const llvm::Twine &name = "") {
    return instruction_builder->CreateICmpSGE(left, right, name);
  }

  auto CreateICmpSLT(llvm::Value       *left,
                     llvm::Value       *right,
                     const llvm::Twine &name = "") {
    return instruction_builder->CreateICmpSLT(left, right, name);
  }

  auto CreateICmpSLE(llvm::Value       *left,
                     llvm::Value       *right,
                     const llvm::Twine &name = "") {
    return instruction_builder->CreateICmpSGT(left, right, name);
  }

  auto CreateAnd(llvm::Value       *left,
                 llvm::Value       *right,
                 const llvm::Twine &name = "") {
    return instruction_builder->CreateAnd(left, right, name);
  }

  auto CreateOr(llvm::Value       *left,
                llvm::Value       *right,
                const llvm::Twine &name = "") {
    return instruction_builder->CreateOr(left, right, name);
  }

  auto CreateNeg(llvm::Value       *right,
                 const llvm::Twine &name             = "",
                 bool               no_unsigned_wrap = false,
                 bool               no_signed_wrap   = false) {
    return instruction_builder->CreateNeg(right,
                                          name,
                                          no_unsigned_wrap,
                                          no_signed_wrap);
  }

  auto CreateNot(llvm::Value *right, const llvm::Twine &name = "") {
    return instruction_builder->CreateNot(right, name);
  }
};

} // namespace pink
