#include "aux/Flags.h"

namespace pink {

auto TypecheckFlags::OnTheLHSOfAssignment() const -> bool {
  return flags[LHSOfAssignment];
}

auto TypecheckFlags::OnTheLHSOfAssignment(bool state) -> bool {
  return flags[LHSOfAssignment] = state;
}

auto TypecheckFlags::WithinAddressOf() const -> bool {
  return flags[inAddressOf];
}

auto TypecheckFlags::WithinAddressOf(bool state) -> bool {
  return flags[inAddressOf] = state;
}

auto TypecheckFlags::WithinDereferencePtr() const -> bool {
  return flags[inDereferencePtr];
}

auto TypecheckFlags::WithinDereferencePtr(bool state) -> bool {
  return flags[inDereferencePtr] = state;
}

auto TypecheckFlags::WithinBindExpression() const -> bool {
  return flags[inBindExpression];
}

auto pink::TypecheckFlags::WithinBindExpression(bool state) -> bool {
  return flags[inBindExpression] = state;
}
} // namespace pink
