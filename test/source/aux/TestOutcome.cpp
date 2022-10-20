
#include "aux/TestOutcome.h"
#include "Test.h"
#include "aux/Outcome.h"

auto TestOutcome(std::ostream &out) -> bool {
  bool result = true;
  out << "\n-----------------------\n";
  out << "Testing pink::Outcome: \n";

  pink::Outcome<int, char> outcome_0(1);
  pink::Outcome<int, char> outcome_1('g');

  result &=
      Test(out, "Outcome<T, U>::Outcome(T t)::operator bool()", outcome_0);
  // when Two is held, operator bool returns false
  result &=
      Test(out, "Outcome<T, U>::Outcome(U u)::operator bool()", !outcome_1);

  int integer = outcome_0.GetFirst();

  result &= Test(out, "Outcome<T, U>::GetOne()", integer == 1);

  char character = outcome_1.GetSecond();

  result &= Test(out, "Outcome<T, U>::GetTwo()", character == 'g');

  result &= Test(out, "pink::Outcome", result);
  out << "\n-----------------------\n";
  return result;
}
