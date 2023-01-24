/**
 * @file Type.h
 * @brief Header for class Type
 * @version 0.1
 */
#pragma once
#include <string>

#include "aux/Outcome.h"

#include "llvm/IR/Type.h"

#include "type/visitor/TypeVisitor.h"

namespace pink {
class Environment;

/**
 * @brief Represents an instance of a Type
 *
 * \note Type is pure virtual
 */
class Type {
public:
  using Pointer = Type *;

  /**
   * @brief Type::Kind is deifined so as to conform to LLVM style [RTTI]
   *
   * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
   *
   */
  enum class Kind {
    Array,
    Boolean,
    Character,
    Function,
    Integer,
    Nil,
    Pointer,
    Slice,
    Tuple,
    Void,
  };

private:
  Kind kind;

public:
  Type(Kind kind) noexcept : kind(kind) {}
  virtual ~Type() noexcept = default;
  Type(const Type &other) noexcept = default;
  Type(Type &&other) noexcept = default;
  auto operator=(const Type &other) noexcept -> Type & = default;
  auto operator=(Type &&other) noexcept -> Type & = default;

  [[nodiscard]] auto GetKind() const -> Kind;

  virtual void Accept(TypeVisitor *vistor) noexcept = 0;
  virtual void Accept(ConstTypeVisitor *visitor) const noexcept = 0;
};
} // namespace pink
