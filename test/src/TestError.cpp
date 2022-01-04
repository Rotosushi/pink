
#include "TestError.hpp"

#include "Error.hpp"

bool TestError(std::ostream& out)
{
    bool result = true;
    out << "\n-----------------------\n";
    out << "Testing Pink::Error: \n";

    std::string edsc = "A sample error description";
    std::string etxt = "A sample error input text";

    // build an error around the given sample description,
    // with a chosen error location
    pink::Error synerr(pink::Error::Kind::Syntax, edsc, pink::Location(0, 2, 0, 7));

    // builds a single string containing the error description along with
    // the passed error text, also underlines the part of the error text
    // according to the errors location.
    std::string synstr = synerr.ToString(etxt);

    // test that ToString returned something,
    // this test does not check that the correct peice
    // of the input is underlined. To do so requires regex
    // or a parser at a minimum, #TODO
    if (synstr.length() == 0)
    {
        result = false;
        out << "\tTest: Syntax Error, Holds Text: Failed\n";
    }
    else
    {
        //out << synstr;
        out << "\tTest: Syntax Error, Holds Text: Passed\n";
    }

    pink::Error typerr(pink::Error::Kind::Type, edsc, pink::Location(0, 9, 0, 13));

    std::string typstr = typerr.ToString(etxt);

    if (typstr.length() == 0)
    {
        result = false;
        out << "\tTest: Type Error, Holds Text: Failed\n";
    }
    else
    {
        //out << typstr;
        out << "\tTest: Type Error, Holds Text: Passed\n";
    }

    pink::Error semerr(pink::Error::Kind::Semantic, edsc, pink::Location(0, 15, 0, 19));

    std::string semstr = semerr.ToString(etxt);

    if (semstr.length() == 0)
    {
        result = false;
        out << "\tTest: Semantic Error: Failed\n";
    }
    else
    {
        //out << semstr;
        out << "\tTest: Semantic Error: Passed\n";
    }


    if (result)
        out << "Test pink::Error: Passed";
    else
        out << "Test pink::Error: Failed";
    out << "\n------------------------\n";
    return result;
}
