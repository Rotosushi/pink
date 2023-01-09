/**
 * @file Type.h
 * @brief Header for class Type
 * @version 0.1
 */
#pragma once
#include <string>

#include "aux/Outcome.h"

#include "llvm/IR/Type.h"

namespace pink {
class Environment;

/**
 * @brief Represents an instance of a Type
 *
 * \note Type is pure virtual, so there is no way to construct a plain Type,
 * only an instance of a derived Type may be constructed
 */
class Type {
public:
  /**
   * @brief Type::Kind is deifined so as to conform to LLVM style [RTTI]
   *
   * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
   *
   * Type::Kind is used as a tag to identify which kind of type that a given
   * Type* points to.
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
  /**
   * @brief The kind of this particular Type
   *
   */
  Kind kind;

public:
  /**
   * @brief Construct a new Type
   *
   * @param kind the kind of Type being constructed
   */
  Type(Kind kind);

  /**
   * @brief Destroy the Type
   *
   */
  virtual ~Type() = default;

  Type(const Type &other) = default;

  Type(Type &&other) = default;

  auto operator=(const Type &other) -> Type & = default;

  auto operator=(Type &&other) -> Type & = default;

  /**
   * @brief Get the Kind of this Type
   *
   * @return Kind the Type::Kind of this Type
   */
  [[nodiscard]] auto GetKind() const -> Kind;

  /**
   * @brief Computes Equality of this type and other type.
   *
   * \note since types are interned, we can use pointer
   *  comparison as a replacement for simple equality.
   *  However, types still need to be compared
   *  structurally, for instance within the type interner
   *  itself. This is what is accomplished with EqualTo
   *
   * @param other the other type to compare against
   * @return true if other *is* equivalent to this type
   * @return false if other *is not* equivalent to this type
   */
  virtual auto EqualTo(Type *other) const -> bool = 0;

  /**
   * @brief Computes the cannonical string representation of this Type
   *
   * @return std::string the string representation
   */
  [[nodiscard]] virtual auto ToString() const -> std::string = 0;

  /**
   * @brief Computes the llvm::Type equivalent to this Type
   *
   * @param env the environment of this compilation unit
   * @return llvm::Type* the llvm::Type equivalent of this type
   */
  [[nodiscard]] virtual auto ToLLVM(const Environment &env) const
      -> llvm::Type * = 0;
};
} // namespace pink
