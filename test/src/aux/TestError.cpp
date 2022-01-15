#include "Test.h"
#include "aux/TestError.h"

#include "aux/Error.h"

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
    result &= Test(out, "Error::Error(Error::Kind::Syntax)", synstr.length() > 0);

    pink::Error typerr(pink::Error::Kind::Type, edsc, pink::Location(0, 9, 0, 13));

    std::string typstr = typerr.ToString(etxt);

    result &= Test(out, "Error::Error(Error::Kind::Type)", typstr.length() > 0);


    pink::Error semerr(pink::Error::Kind::Semantic, edsc, pink::Location(0, 15, 0, 19));

    std::string semstr = semerr.ToString(etxt);

    result &= Test(out, "Error::Error(Error::Kind::Semantic)", semstr.length() > 0);



    if (result)
        out << "Test pink::Error: Passed";
    else
        out << "Test pink::Error: Failed";
    out << "\n------------------------\n";
    return result;
}
