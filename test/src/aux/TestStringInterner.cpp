
#include "aux/TestStringInterner.hpp"

#include "aux/StringInterner.hpp"

/*
    This unit test might be a little redundant given that
    the only functionality the class provides is precisely
    equivalent to a subset of llvm::StringSet<>, and llvm
    themselves definetly run unit tests.
*/
bool TestStringInterner(std::ostream& out)
{
    bool result = true;
    out << "\n-----------------------\n";
    out << "Testing Pink::StringInterner: \n";

    pink::StringInterner interner;
    std::string s0 = "sample-variable";
    std::string s1 = "another-variable";

    pink::InternedString istr0 = interner.Intern(s0);

    /*
        Checks that the string interner returns a valid
        InternedString, that holds the same characters as
        the argument string.
    */
    if (istr0 != NULL && s0 == istr0)
    {
        out << "\tTest StringInterner::Intern(std::string&), unique string is valid: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest StringInterner::Intern(std::string&), unique string is valid: Failed\n";
    }

    pink::InternedString istr1 = interner.Intern(s1);

    /*
        Duplicates the above checks on a second InternedString, then checks
        that the StringInterner returned a different InternedString
        from the first InternedString by pointer comparison
    */
    if (istr1 != NULL && s1 == istr1 && istr1 != istr0)
    {
        out << "\tTest StringInterner::Intern(std::string&), two unique strings compare not-equal: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest StringInterner::Intern(std::string&), two unique strings compare not-equal: Failed\n";
    }

    pink::InternedString istr2 = interner.Intern(s0);

    /*
        Duplicates the first checks on a duplicate InternedString,
        then checks that the StringInterner returned the exact same
        InternedString as the first InternedString, due to the duplicate text.
    */
    if (istr2 != NULL && s0 == istr2 && istr2 == istr0)
    {
        out << "\tTest StringInterner::Intern(std::string&), duplicate string compares equal: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest StringInterner::Intern(std::string&), duplicate string compares equal: Failed\n";
    }

    if (result)
        out << "Test: pink::StringInterner: Passed\n";
    else
        out << "Test: pink::StringInterner: Failed\n";

    out << "\n-----------------------\n";
    return result;
}
