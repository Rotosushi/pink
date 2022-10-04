#include "aux/Flags.h"

namespace pink {

auto Flags::OnTheLHSOfAssignment() const -> bool {
  return flags[LHSOfAssignment];
}

auto Flags::OnTheLHSOfAssignment(bool state) -> bool {
  return flags[LHSOfAssignment] = state;
}

auto Flags::WithinAddressOf() const -> bool { return flags[inAddressOf]; }

auto Flags::WithinAddressOf(bool state) -> bool {
  return flags[inAddressOf] = state;
}

auto Flags::WithinDereferencePtr() const -> bool {
  return flags[inDereferencePtr];
}

auto Flags::WithinDereferencePtr(bool state) -> bool {
  return flags[inDereferencePtr] = state;
}
} // namespace pink
