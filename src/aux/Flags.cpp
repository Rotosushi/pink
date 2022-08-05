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
}
