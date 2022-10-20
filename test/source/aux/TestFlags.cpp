#include "aux/TestFlags.h"

#include "Test.h"

#include "aux/Flags.h"

auto TestFlags(std::ostream &out) -> bool {
  bool result = true;
  out << "\n-----------------------------------------\n";
  out << "Testing pink::TypecheckFlags\n";

  pink::TypecheckFlags flags;

  flags.OnTheLHSOfAssignment(true);

  result &= Test(out, "TypecheckFlags::OnTheLHSOfAssignment()",
                 flags.OnTheLHSOfAssignment());

  flags.WithinAddressOf(true);

  result &=
      Test(out, "TypecheckFlags::WithinAddressOf()", flags.WithinAddressOf());

  flags.WithinDereferencePtr(true);

  result &= Test(out, "TypecheckFlags::WithinDereferencePtr()",
                 flags.WithinDereferencePtr());

  return Test(out, "pink::TypecheckFlags", result);
}