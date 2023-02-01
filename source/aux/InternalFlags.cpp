#include "aux/InternalFlags.h"

namespace pink {

auto InternalFlags::OnTheLHSOfAssignment() const -> bool {
  return set[LHSOfAssignment];
}

auto InternalFlags::OnTheLHSOfAssignment(bool state) -> bool {
  return set[LHSOfAssignment] = state;
}

auto InternalFlags::WithinAddressOf() const -> bool { return set[inAddressOf]; }

auto InternalFlags::WithinAddressOf(bool state) -> bool {
  return set[inAddressOf] = state;
}

auto InternalFlags::WithinDereferencePtr() const -> bool {
  return set[inDereferencePtr];
}

auto InternalFlags::WithinDereferencePtr(bool state) -> bool {
  return set[inDereferencePtr] = state;
}

auto InternalFlags::WithinBindExpression() const -> bool {
  return set[inBindExpression];
}

auto pink::InternalFlags::WithinBindExpression(bool state) -> bool {
  return set[inBindExpression] = state;
}
} // namespace pink
