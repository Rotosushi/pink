/**
 * @file FunctionType.h
 * @brief Header for class FunctionType
 * @version 0.1
 */
#pragma once
#include <vector>

#include "type/TypeInterface.h"

namespace pink {
/**
 * @brief Represents the Type of a Function
 */
class FunctionType : public TypeInterface {
public:
  using Arguments      = std::vector<TypeInterface::Pointer>;
  using iterator       = Arguments::iterator;
  using const_iterator = Arguments::const_iterator;
  using Pointer        = FunctionType const *;

private:
  TypeInterface::Pointer return_type;
  Arguments              arguments;

public:
  FunctionType(TypeInterner          *context,
               TypeInterface::Pointer return_type,
               Arguments              arguments) noexcept
      : TypeInterface(TypeInterface::Kind::Function, context),
        return_type(return_type),
        arguments(std::move(arguments)) {
    assert(return_type != nullptr);
  }
  ~FunctionType() noexcept override                = default;
  FunctionType(const FunctionType &other) noexcept = default;
  FunctionType(FunctionType &&other) noexcept      = default;
  auto operator=(const FunctionType &other) noexcept
      -> FunctionType                                           & = default;
  auto operator=(FunctionType &&other) noexcept -> FunctionType & = default;

  [[nodiscard]] auto GetReturnType() noexcept -> TypeInterface::Pointer {
    return return_type;
  }
  [[nodiscard]] auto GetReturnType() const noexcept -> TypeInterface::Pointer {
    return return_type;
  }
  [[nodiscard]] auto GetArguments() noexcept -> Arguments & {
    return arguments;
  }
  [[nodiscard]] auto GetArguments() const noexcept -> const Arguments & {
    return arguments;
  }

  [[nodiscard]] auto begin() noexcept -> iterator { return arguments.begin(); }
  [[nodiscard]] auto begin() const noexcept -> const_iterator {
    return arguments.begin();
  }
  [[nodiscard]] auto cbegin() const noexcept -> const_iterator {
    return arguments.cbegin();
  }
  [[nodiscard]] auto end() noexcept -> iterator { return arguments.end(); }
  [[nodiscard]] auto end() const noexcept -> const_iterator {
    return arguments.end();
  }
  [[nodiscard]] auto cend() const noexcept -> const_iterator {
    return arguments.cend();
  }

  static auto classof(const TypeInterface *type) noexcept -> bool {
    return TypeInterface::Kind::Function == type->GetKind();
  }

  void Accept(TypeVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstTypeVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
