
#include "aux/TestTypeInterner.h"
#include "Test.h"
#include "aux/TypeInterner.h"

bool TestTypeInterner(std::ostream &out) {
  bool        result = true;
  std::string name   = "pink::TypeInterner";
  TestHeader(out, name);

  pink::TypeInterner  interner;
  pink::Type::Pointer nt0 = interner.GetNilType();

  result &= Test(out, "TypeInterner::GetNilType()", nt0 != nullptr);

  pink::Type::Pointer nt1 = interner.GetNilType();

  result &=
      Test(out, "TypeInterner::GetNilType(), interned type compares equal",
           nt1 != nullptr && nt0 == nt1);

  pink::Type::Pointer bt0 = interner.GetBoolType();

  result &= Test(
      out, "TypeInterner::GetBoolType(), different types compare not equal",
      bt0 != nullptr && nt0 != bt0);

  pink::Type::Pointer it0 = interner.GetIntType();

  result &=
      Test(out, "TypeInterner::GetIntType(), different types compare not equal",
           it0 != nullptr && it0 != bt0);

  std::vector<pink::Type::Pointer> arg_tys0({bt0, bt0});

  pink::Type::Pointer ft0 = interner.GetFunctionType(bt0, arg_tys0);

  result &= Test(out, "TypeInterner::GetFunctionType()", ft0 != nullptr);

  pink::Type::Pointer ft1 = interner.GetFunctionType(bt0, arg_tys0);

  result &=
      Test(out, "TypeInterner::GetFunctionType(), same types compare equal",
           ft1 != nullptr && ft1 == ft0);

  std::vector<pink::Type::Pointer> arg_tys1({it0, it0});

  pink::Type::Pointer ft2 = interner.GetFunctionType(it0, arg_tys1);

  result &= Test(
      out, "TypeInterner::GetFunctionType(), Different Types Compare Not Equal",
      ft2 != nullptr && ft2 != ft1);

  return TestFooter(out, name, result);
}
