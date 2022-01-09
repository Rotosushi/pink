#include "Test.hpp"
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

    result &= Test(out, "SymbolTable::OuterScope(), global scope", t0.OuterScope() == nullptr);

    result &= Test(out, "SymbolTable::OuterScope(), inner scope", t1.OuterScope() == &(t0));

    pink::Location l(0, 5, 0, 7);
    pink::Nil* n0 = new pink::Nil(l);
    pink::InternedString x = interner.Intern("x");

    // SymbolTable::Bind assumes ownership of the passed in Ast*
    t0.Bind(x, n0);
    llvm::Optional<pink::Ast*> s0 = t0.Lookup(x);

    result &= Test(out, "SymbolTable::Bind(), SymbolTable::Lookup()", s0.hasValue() && *s0 == n0);

    t0.Unbind(x);
    llvm::Optional<pink::Ast*> s1 = t0.Lookup(x);

    result &= Test(out, "SymbolTable::Unbind()", !s1.hasValue());

    result &= Test(out, "pink::SymbolTable", result);
    out << "\n-----------------------\n";
    return result;
}
