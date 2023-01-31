#include "aux/TestStringInterner.h"
#include "Test.h"

#include "aux/StringInterner.h"

/*
    This unit test might be a little redundant given that
    the only functionality the class provides is precisely
    equivalent to a subset of llvm::StringSet<>, and llvm
    themselves definetly run unit tests.
*/
bool TestStringInterner(std::ostream &out) {
  bool        result = true;
  std::string name   = "pink::StringInterner";
  TestHeader(out, name);

  pink::StringInterner interner;
  std::string          s0 = "sample-variable";
  std::string          s1 = "another-variable";

  pink::InternedString istr0 = interner.Intern(s0);

  result &= Test(out, "StringInterner::Intern(), unique string",
                 istr0 != nullptr && s0 == istr0);

  pink::InternedString istr1 = interner.Intern(s1);

  result &= Test(out, "StringInterner::Intern(), second uniqe string",
                 istr1 != nullptr && s1 == istr1 && istr1 != istr0);

  pink::InternedString istr2 = interner.Intern(s0);

  result &= Test(out, "StringInterner::Intern(), duplicate string",
                 istr2 != nullptr && s0 == istr2 && istr2 == istr0);

  return TestFooter(out, name, result);
}
