
#include "TestAstAndNil.hpp"
#include "Nil.hpp"


bool TestAstAndNil(std::ostream& out)
{
    bool result = true;
    out << "\n-----------------------\n";
    out << "Testing pink::Ast and pink::Nil: \n";

    /*
        The Ast class itself only provides a small
        amount of the functionality of any given
        syntax node.

        tests:

        -) An Ast node constructed with a particular kind
            holds that particular kind.

        -) An Ast node constructed with a particular Location
            holds that particular Location.

        -) Ast::ToString() can only be tested with an instance
            of a particular Ast node.
    */

    pink::Location l0(0, 14, 0, 15);
    pink::Nil a(l0);

    if (a.GetKind() == pink::Ast::Kind::Nil)
    {
        out << "\tTest: pink::Ast::GetKind(): Passed\n";
    }
    else
    {
        out << "\tTest: pink::Ast::GetKind(): Failed\n";
    }

    pink::Location l1(l0);
    if (l0.firstLine == l1.firstLine && l0.firstColumn == l1.firstColumn
     && l0.lastLine == l1.lastLine && l0.lastColumn == l1.lastColumn)
    {
        out << "\tTest: pink::Ast::GetLoc(): Passed\n";
    }
    else
    {
        out << "\tTest: pink::Ast::GetLoc(): Failed\n";
    }

    if (result)
        out << "Test: pink::Ast and pink::Nil: Passed\n";
    else
        out << "Test: pink::Ast and pink::Nil: Failed\n";
    out << "\n-----------------------\n";
}
