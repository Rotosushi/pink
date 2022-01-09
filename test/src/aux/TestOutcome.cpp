
#include "Test.hpp"
#include "aux/TestOutcome.hpp"
#include "aux/Outcome.hpp"

bool TestOutcome(std::ostream& out)
{
    bool result = true;
    out << "\n-----------------------\n";
    out << "Testing pink::Outcome: \n";

    pink::Outcome<int, char> o0(1);
    pink::Outcome<int, char> o1((char)'g');

    result &= Test(out, "Outcome<T, U>::Outcome(T t)::operator bool()", o0);
    // when Two is held, operator bool returns false
    result &= Test(out, "Outcome<T, U>::Outcome(U u)::operator bool()", !o1);

    int i0 = o0.GetOne();

    result &= Test(out, "Outcome<T, U>::GetOne()", i0 == 1);


    char c1 = o1.GetTwo();

    result &= Test(out, "Outcome<T, U>::GetTwo()", c1 == 'g');

    result &= Test(out, "pink::Outcome", result);
    out << "\n-----------------------\n";
    return result;
}
