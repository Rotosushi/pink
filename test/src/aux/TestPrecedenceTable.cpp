#include "Test.hpp"
#include "aux/TestPrecedenceTable.hpp"
#include "aux/PrecedenceTable.hpp"


bool TestPrecedenceTable(std::ostream& out)
{
    bool result = true;
    out << "\n-----------------------\n";
    out << "Testing and pink::PrecedenceTable: \n";

    pink::PrecedenceTable pt;
    pink::StringInterner strings;

    pink::InternedString plus = strings.Intern("+");
    pink::InternedString minus = strings.Intern("-");

    pt.Register(plus, pink::Associativity::Left, 5);

    llvm::Optional<std::pair<pink::Associativity, pink::Precedence>> pair = pt.Lookup(plus);

    result &= Test(out, "PrecedenceTable::Lookup()", pair.hasValue() && (std::get<0>(*pair) == pink::Associativity::Left) && (std::get<1>(*pair) == 5));

    llvm::Optional<std::pair<pink::Associativity, pink::Precedence>> unk = pt.Lookup(minus);

    result &= Test(out, "PrecedenceTable::Lookup()", !unk.hasValue());

    result &= Test(out, "pink::PrecedenceTable", result);
    out << "\n-----------------------\n";
    return result;
}
