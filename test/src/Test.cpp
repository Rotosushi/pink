
#include "Test.hpp"

#include "aux/TestError.hpp"
#include "aux/TestOutcome.hpp"
#include "aux/TestStringInterner.hpp"
#include "aux/TestTypeInterner.hpp"
#include "aux/TestSymbolTable.hpp"
#include "aux/TestEnvironment.hpp"
#include "aux/TestPrecedenceTable.hpp"
#include "aux/TestUnopCodegen.hpp"
#include "aux/TestUnopLiteral.hpp"

#include "ast/TestAstAndNil.hpp"
#include "ast/TestBool.hpp"
#include "ast/TestInt.hpp"
#include "ast/TestVariable.hpp"
#include "ast/TestBind.hpp"
#include "ast/TestBinop.hpp"
#include "ast/TestUnop.hpp"

#include "type/TestTypeAndNilType.hpp"
#include "type/TestIntType.hpp"
#include "type/TestBoolType.hpp"

#include "front/TestToken.hpp"
#include "front/TestLexer.hpp"


/*
    This is a super basic funclet, which simply saves
    me from having to type out so much code in every
    Test...() body
*/
bool Test(std::ostream& out, std::string test_name, bool test)
{
    if (test)
    {
        out << "\tTest: " << test_name << ": Passed\n";
    }
    else
    {
        out << "\tTest: " << test_name << ": Failed\n";
    }

    return test;
}


size_t RunTests(std::ostream& out, size_t flags)
{
    size_t result = 0;

    /*
        If we refactor the basic test subroutine to take
        a reference to the class being tested, then we
        can rerun previous tests when we have a component
        of a class that is itself an instance of that
        other class.

        In this way we could compose together the unit tests
        themselves to write tests for the classes that are
        composed of other classes. This would repeat work.
    */

    /*
        Auxilliary Classes Tests.
    */
    if ((flags & TEST_ERROR) > 0)
    {
        if (TestError(out))
            result |= TEST_ERROR;
    }

    if ((flags & TEST_OUTCOME) > 0)
    {
        if (TestOutcome(out))
            result |= TEST_OUTCOME;
    }

    if ((flags & TEST_STRING_INTERNER) > 0)
    {
        if (TestStringInterner(out))
            result |= TEST_STRING_INTERNER;
    }

    if ((flags & TEST_SYMBOL_TABLE) > 0)
    {
        if (TestSymbolTable(out))
            result |= TEST_SYMBOL_TABLE;
    }

    if ((flags & TEST_TYPE_INTERNER) > 0)
    {
        if (TestTypeInterner(out))
            result |= TEST_TYPE_INTERNER;
    }

    if ((flags & TEST_ENVIRONMENT) > 0)
    {
        if (TestEnvironment(out))
            result |= TEST_ENVIRONMENT;
    }

    if ((flags & TEST_PRECEDENCE_TABLE) > 0)
    {
        if (TestPrecedenceTable(out))
            result |= TEST_PRECEDENCE_TABLE;
    }

    if ((flags & TEST_UNOP_CODEGEN) > 0)
    {
        if (TestUnopCodegen(out))
            result |= TEST_UNOP_CODEGEN;
    }

    if ((flags & TEST_UNOP_LITERAL) > 0)
    {
        if (TestUnopLiteral(out))
            result |= TEST_UNOP_LITERAL;
    }


    /*
        Abstract Syntax Tree Tests
    */
    if ((flags & TEST_AST_AND_NIL) > 0)
    {
        if (TestAstAndNil(out))
            result |= TEST_AST_AND_NIL;
    }

    if ((flags & TEST_BOOL) > 0)
    {
        if (TestBool(out))
            result |= TEST_BOOL;
    }

    if ((flags & TEST_INT) > 0)
    {
        if (TestInt(out))
            result |= TEST_INT;
    }

    if ((flags & TEST_VARIABLE) > 0)
    {
        if (TestVariable(out))
            result |= TEST_VARIABLE;
    }

    if ((flags & TEST_BIND) > 0)
    {
        if (TestBind(out))
            result |= TEST_BIND;
    }

    if((flags & TEST_BINOP) > 0)
    {
        if (TestBinop(out))
            result |= TEST_BINOP;
    }

    if ((flags & TEST_UNOP) > 0)
    {
        if (TestUnop(out))
            result |= TEST_UNOP;
    }


    /*
        Type Tests
    */
    if ((flags & TEST_TYPE_AND_NIL_TYPE) > 0)
    {
        if (TestTypeAndNilType(out))
            result |= TEST_TYPE_AND_NIL_TYPE;
    }

    if ((flags & TEST_INT_TYPE) > 0)
    {
        if (TestIntType(out))
            result |= TEST_INT_TYPE;
    }

    if ((flags & TEST_BOOL_TYPE) > 0)
    {
        if (TestBoolType(out))
            result |= TEST_BOOL_TYPE;
    }

    /*
        Frontend Tests
    */
    if ((flags & TEST_TOKEN) > 0)
    {
        if (TestToken(out))
            result |= TEST_TOKEN;
    }

    if ((flags & TEST_LEXER) > 0)
    {
        if (TestLexer(out))
            result |= TEST_LEXER;
    }



    return result;
}


/*
    This function is not exported fron this file
    because it's arguments are a bit confusing,
    the first size_t (flags) is for the full word of test results,
    whereas the test_flag, the second size_t is for
    which individual test we are checking for success/failure.

    This sort of routine might be able to be adapted to make
    for a sort of testing monad. which could be used to make
    the rest of the testing procedures less verbose.
*/
void PrintPassedTest(std::ostream& out, size_t flags, size_t test_flag, std::string test_name)
{
    if ((flags & test_flag) > 0)
    {
        out << "Test: " << test_name << ": Passed\n";
    }
    else
    {
        out << "Test: " << test_name << ": Failed\n";
    }

}


void PrintPassedTests(std::ostream& out, size_t flags)
{
    /*
        Auxilliary Classes Tests
    */
    PrintPassedTest(out, flags, TEST_ERROR, "pink::Error");
    PrintPassedTest(out, flags, TEST_OUTCOME, "pink::Outcome");
    PrintPassedTest(out, flags, TEST_STRING_INTERNER, "pink::StringInterner");
    PrintPassedTest(out, flags, TEST_SYMBOL_TABLE, "pink::SymbolTable");
    PrintPassedTest(out, flags, TEST_TYPE_INTERNER, "pink::TypeInterner");
    PrintPassedTest(out, flags, TEST_ENVIRONMENT, "pink::Environment");
    PrintPassedTest(out, flags, TEST_PRECEDENCE_TABLE, "pink::PrecedenceTable");
    PrintPassedTest(out, flags, TEST_UNOP_CODEGEN, "pink::UnopCodegen");
    PrintPassedTest(out, flags, TEST_UNOP_LITERAL, "pink::UnopLiteral");

    /*
        Abstract Syntax Tree Tests
    */
    PrintPassedTest(out, flags, TEST_AST_AND_NIL, "pink::Ast, pink::Nil");
    PrintPassedTest(out, flags, TEST_BOOL, "pink::Bool");
    PrintPassedTest(out, flags, TEST_INT, "pink::Int");
    PrintPassedTest(out, flags, TEST_VARIABLE, "pink::Variable");
    PrintPassedTest(out, flags, TEST_BIND, "pink::Bind");
    PrintPassedTest(out, flags, TEST_BINOP, "pink::Binop");
    PrintPassedTest(out, flags, TEST_UNOP, "pink::Unop");

    /*
        Type Tests
    */
    PrintPassedTest(out, flags, TEST_TYPE_AND_NIL_TYPE, "pink::Type, pink::NilType");
    PrintPassedTest(out, flags, TEST_INT_TYPE, "pink::IntType");
    PrintPassedTest(out, flags, TEST_BOOL_TYPE, "pink::BoolType");

    /*
        Frontend Tests
    */
    PrintPassedTest(out, flags, TEST_TOKEN, "pink::Token");
    PrintPassedTest(out, flags, TEST_LEXER, "pink::Lexer");

}
