/**
 * @file Type.h
 * @brief Header for class Type
 * @version 0.1
 */
#pragma once
#include <string>
#include <variant>

#include "aux/Outcome.h"

#include "llvm/IR/Type.h"

#include "type/visitor/TypeVisitor.h"

namespace pink {
class Environment;
class TypeInterner;

/**
 * @brief Represents an instance of a Type
 *
 * \note Type is pure virtual
 *
 * \todo using Type =
 * std::variant<
 *   ArrayType,
 *   BooleanType,
 *   CharacterType,
 *   FunctionType,
 *   IntegerType,
 *   NilType,
 *   PointerType,
 *   SliceType,
 *   TupleType,
 *   VoidType
 *   >
 */
class TypeInterface {
public:
  using Pointer = TypeInterface const *;

  /**
   * @brief Type::Kind is defined so as to conform to LLVM style [RTTI]
   *
   * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
   *
   */
  enum class Kind {
    Array,
    Boolean,
    Character,
    Function,
    Identifier,
    Integer,
    Nil,
    Pointer,
    Slice,
    Tuple,
    Void,
  };

private:
  Kind                kind;
  mutable llvm::Type *llvm_type;
  TypeInterner       *context;

public:
  TypeInterface(Kind kind, TypeInterner *context) noexcept
      : kind{kind},
        llvm_type{nullptr},
        context(context) {
    assert(context != nullptr);
  }
  virtual ~TypeInterface() noexcept                  = default;
  TypeInterface(const TypeInterface &other) noexcept = default;
  TypeInterface(TypeInterface &&other) noexcept      = default;
  auto operator=(const TypeInterface &other) noexcept
      -> TypeInterface                                            & = default;
  auto operator=(TypeInterface &&other) noexcept -> TypeInterface & = default;

  [[nodiscard]] auto GetKind() const -> Kind { return kind; }
  [[nodiscard]] auto GetContext() const -> TypeInterner * { return context; }

  void SetCachedLLVMType(llvm::Type *llvm_type) const noexcept {
    this->llvm_type = llvm_type;
  }

  auto CachedLLVMType() const noexcept -> std::optional<llvm::Type *> {
    if (llvm_type == nullptr) {
      return {};
    }
    return {llvm_type};
  }

  auto CachedLLVMTypeOrAssert() const noexcept -> llvm::Type * {
    assert(llvm_type != nullptr);
    return llvm_type;
  }

  virtual void Accept(TypeVisitor *vistor) noexcept             = 0;
  virtual void Accept(ConstTypeVisitor *visitor) const noexcept = 0;
};
} // namespace pink
