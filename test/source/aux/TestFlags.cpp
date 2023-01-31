#include "aux/TestFlags.h"

#include "Test.h"

#include "aux/Flags.h"

auto TestFlags(std::ostream &out) -> bool {
  bool        result = true;
  std::string name   = "pink::TypecheckFlags";
  TestHeader(out, name);

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

  return TestFooter(out, name, result);
}