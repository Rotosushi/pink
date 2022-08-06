#include "aux/Flags.h"

namespace pink {

  Flags::Flags()
  {

  }

  Flags::~Flags()
  {

  }

  bool Flags::OnTheLHSOfAssignment()
  {
    return flags[LHSOfAssignment];
  }

  bool Flags::OnTheLHSOfAssignment(bool state)
  {
    return flags[LHSOfAssignment] = state;
  }

  bool Flags::WithinAddressOf()
  {
    return flags[inAddressOf];
  }

  bool Flags::WithinAddressOf(bool state)
  {
    return flags[inAddressOf] = state;
  }

  bool Flags::WithinDereferencePtr()
  {
    return flags[inDereferencePtr];
  }

  bool Flags::WithinDereferencePtr(bool state)
  {
    return flags[inDereferencePtr] = state;
  }
}
