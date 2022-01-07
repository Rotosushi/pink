

#include "aux/TestOutcome.hpp"
#include "aux/Outcome.hpp"

bool TestOutcome(std::ostream& out)
{
    bool result = true;
    out << "\n-----------------------\n";
    out << "Testing and pink::Outcome: \n";

    pink::Outcome<int, char> o0(1);
    pink::Outcome<int, char> o1((char)'g');

    if (o0)
    {
        out << "\tTest: Outcome::Outcome(T t), Outcome::operator bool: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Outcome::Outcome(T t), Outcome::operator bool: Failed\n";
    }

    if (o1)
    {
        result = false;
        out << "\tTest: Outcome::Outcome(U u), Outcome::operator bool: Failed\n";
    }
    else
    {
        out << "\tTest: Outcome::Outcome(U u), Outcome::operator bool: Passed\n";
    }

    int i0 = o0.GetOne();

    if (i0 == 1)
    {
        out << "\tTest: Outcome::GetOne(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Outcome::GetOne(): Failed\n";
    }

    char c1 = o1.GetTwo();

    if (c1 == 'g')
    {
        out << "\tTest: Outcome::GetTwo(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Outcome::GetTwo(): Failed\n";
    }

    if (result)
        out << "Test: pink::Outcome: Passed\n";
    else
        out << "Test: pink::Outcome: Failed\n";
    out << "\n-----------------------\n";
    return result;
}
