
#include "Test.hpp"

#include "aux/TestError.hpp"
#include "aux/TestOutcome.hpp"
#include "aux/TestStringInterner.hpp"
#include "aux/TestTypeInterner.hpp"
#include "aux/TestSymbolTable.hpp"
#include "aux/TestEnvironment.hpp"

#include "ops/TestUnopCodegen.hpp"
#include "ops/TestUnopLiteral.hpp"
#include "ops/TestUnopTable.hpp"
#include "ops/TestBinopCodegen.hpp"
#include "ops/TestBinopLiteral.hpp"
#include "ops/TestBinopTable.hpp"

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
        If we refactor each test subroutine to take
        a reference or ptr to the class being tested, then we
        can rerun previous tests when we have a component
        of a class that is itself an instance of that
        other class. Which tests more which is better right?

        A cool benefit is that this way we could compose
        together the unit tests themselves to write unit tests
        for the classes that are composed of other classes.

        Thinking it through this would repeat some amount of Tests.
    */

    /*
        Auxilliary Unit Tests.
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

    /*
        Operator Unit Tests
    */
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

    if ((flags & TEST_UNOP_TABLE) > 0)
    {
        if (TestUnopTable(out))
            result |= TEST_UNOP_TABLE;
    }

    if ((flags & TEST_BINOP_CODEGEN) > 0)
    {
        if (TestBinopCodegen(out))
            result |= TEST_BINOP_CODEGEN;
    }

    if ((flags & TEST_BINOP_LITERAL) > 0)
    {
        if (TestBinopLiteral(out))
            result |= TEST_BINOP_LITERAL;
    }

    if ((flags & TEST_BINOP_TABLE) > 0)
    {
        if (TestBinopTable(out))
            result |= TEST_BINOP_TABLE;
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
    This is a simple implementation of this procedure,
    as it always assumes that every test was run.
    and thus any zero's in place of a flag means the
    test failed, but that can also happen when the test
    was not one that was selected to run.
    which works great during development, as we are
    in fact running every test each time. However
    a more robust testing routine/framework
    would probably keep track of which tests were
    requested to run, and then only print which of those
    passed. #TODO: refactor the testing framework to
    accomplish this
*/
void PrintPassedTests(std::ostream& out, size_t flags)
{
    bool result = true;
    /*
        Auxilliary Classes Tests
    */
    result &= Test(out, "pink::Error",          flags & TEST_ERROR);
    result &= Test(out, "pink::Outcome",        flags & TEST_OUTCOME);
    result &= Test(out, "pink::StringInterner", flags & TEST_STRING_INTERNER);
    result &= Test(out, "pink::SymbolTable",    flags & TEST_SYMBOL_TABLE);
    result &= Test(out, "pink::TypeInterner",   flags & TEST_TYPE_INTERNER);
    result &= Test(out, "pink::Environment",    flags & TEST_ENVIRONMENT);
    result &= Test(out, "pink::UnopCodegen",    flags & TEST_UNOP_CODEGEN);
    result &= Test(out, "pink::UnopLiteral",    flags & TEST_UNOP_LITERAL);
    result &= Test(out, "pink::UnopTable",      flags & TEST_UNOP_TABLE);
    result &= Test(out, "pink::BinopCodegen",   flags & TEST_BINOP_CODEGEN);
    result &= Test(out, "pink::BinopLiteral",   flags & TEST_BINOP_LITERAL);
    result &= Test(out, "pink::BinopTable",     flags & TEST_BINOP_TABLE);
    /*
        Abstract Syntax Tree Tests
    */
    result &= Test(out, "pink::Ast, pink::Nil", flags & TEST_AST_AND_NIL);
    result &= Test(out, "pink::Bool",           flags & TEST_BOOL);
    result &= Test(out, "pink::Int",            flags & TEST_INT);
    result &= Test(out, "pink::Variable",       flags & TEST_VARIABLE);
    result &= Test(out, "pink::Bind",           flags & TEST_BIND);
    result &= Test(out, "pink::Binop",          flags & TEST_BINOP);
    result &= Test(out, "pink::Unop",           flags & TEST_UNOP);

    /*
        Type Tests
    */
    result &= Test(out, "pink::Type, pink::NilType", flags & TEST_TYPE_AND_NIL_TYPE);
    result &= Test(out, "pink::IntType",             flags & TEST_INT_TYPE);
    result &= Test(out, "pink::BoolType",            flags & TEST_BOOL_TYPE);

    /*
        Frontend Tests
    */
    result &= Test(out, "pink::Token",  flags & TEST_TOKEN);
    result &= Test(out, "pink::Lexer",  flags & TEST_LEXER);

    Test(out, "pink::Test", result);
}
