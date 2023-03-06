#pragma once

#include "type/Type.h"

namespace pink {
/**
 * @brief The type of slices
 *
 *
 * a slice is a pointer composed together with the
 * size of the allocation it points too.
 *
 * An Array can be coerced into a slice safely.
 * the only consideration is the fact that we
 * store an array as the length plus the array,
 * so a slice pointing to the array will be a
 * copy of the length plus a pointer to the array
 * allocation. Thus using a slice in this way incurs
 * a bit of overhead compared to direct array access
 * within a local scope.
 * however passing an array to a function using a slice
 * is very efficient compared to passing the array
 * itself by value.
 *
 * an array in the return slot of a function, as the language
 * is currently defined must have compile time known size.
 * and then it can be allocated in the calling scope and
 * simply written to in the callee scope. And this should
 * work up a chain of calls.
 *
 */
class SliceType : public Type {
public:
  using Pointer = SliceType const *;

private:
  Type::Pointer pointee_type;

public:
  SliceType(TypeInterner *context, Type::Pointer pointee_type) noexcept
      : Type(Type::Kind::Slice, context),
        pointee_type(pointee_type) {
    assert(pointee_type != nullptr);
  }
  ~SliceType() noexcept override                                 = default;
  SliceType(const SliceType &other) noexcept                     = default;
  SliceType(SliceType &&other) noexcept                          = default;
  auto operator=(const SliceType &other) noexcept -> SliceType & = default;
  auto operator=(SliceType &&other) noexcept -> SliceType      & = default;

  static auto classof(const Type *type) noexcept -> bool {
    return Type::Kind::Slice == type->GetKind();
  }

  [[nodiscard]] auto GetPointeeType() const noexcept -> Type::Pointer {
    return pointee_type;
  }

  void Accept(TypeVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstTypeVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink