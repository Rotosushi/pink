
#include "aux/TestSymbolTable.hpp"

#include "aux/SymbolTable.hpp"
#include "ast/Nil.hpp"


bool TestSymbolTable(std::ostream& out)
{
    /*
        Testing the various functions of the symbol table entails:

        -) SymbolTable's constructed against no outer scope have
            a nullptr for their OuterScope

        -) SymbolTable's Constructed 'inside' of another symbol table
            have a valid pointer to that symbol table

        -) Bind validly binds a symbol to a particular term because
            -) Lookup of an existing symbol returns that symbols binding

        -) Unbind successfully removes a particular binding because
            -) Lookup of a nonexisting symbol returns the empty optional

    */

    bool result = true;
    out << "\n-----------------------\n";
    out << "Testing pink::SymbolTable: \n";

    pink::StringInterner interner;
    pink::SymbolTable t0;
    pink::SymbolTable t1(&t0);

    if (t0.OuterScope() == nullptr)
    {
        out << "\tTest: SymbolTable::OuterScope(), default constructed symbol table: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: SymbolTable::OuterScope(), default constructed symbol table: Failed\n";
    }

    if (t1.OuterScope() == &(t0))
    {
        out << "\tTest: SymbolTable::OuterScope(), inner table, Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: SymbolTable::OuterScope(), inner table, Failed\n";
    }

    pink::Location l(0, 5, 0, 7);
    pink::Nil* n0 = new pink::Nil(l);
    pink::InternedString x = interner.Intern("x");

    // SymbolTable::Bind assumes ownership of the passed in Ast*
    t0.Bind(x, n0);
    llvm::Optional<pink::Ast*> s0 = t0.Lookup(x);

    if (s0.hasValue() && *s0 == n0)
    {
        out << "\tTest: SymbolTable::Bind, and SymbolTable::Lookup(), created and found a binding: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: SymbolTable::Bind, and SymbolTable::Lookup(), created and found a binding: Failed\n";
    }

    t0.Unbind(x);
    llvm::Optional<pink::Ast*> s1 = t0.Lookup(x);

    if (!s1.hasValue())
    {
        out << "\tTest: SymbolTable::Unbind, and SymbolTable::Lookup(), removed and did not find a binding: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: SymbolTable::Unbind, and SymbolTable::Lookup(), removed and did not find a binding: Failed\n";
    }

    if (result)
        out << "Test: pink::SymbolTable: Passed\n";
    else
        out << "Test: pink::SymbolTable: Failed\n";
    out << "\n-----------------------\n";
    return result;
}
