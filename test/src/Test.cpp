
#include "Test.h"

#include "aux/TestError.h"
#include "aux/TestOutcome.h"
#include "aux/TestStringInterner.h"
#include "aux/TestTypeInterner.h"
#include "aux/TestSymbolTable.h"
#include "aux/TestEnvironment.h"

#include "ops/TestUnopCodegen.h"
#include "ops/TestUnopLiteral.h"
#include "ops/TestUnopTable.h"
#include "ops/TestBinopCodegen.h"
#include "ops/TestBinopLiteral.h"
#include "ops/TestBinopTable.h"

#include "ast/TestAstAndNil.h"
#include "ast/TestBool.h"
#include "ast/TestInt.h"
#include "ast/TestVariable.h"
#include "ast/TestBind.h"
#include "ast/TestBinop.h"
#include "ast/TestUnop.h"
#include "ast/TestAssignment.h"
#include "ast/TestBlock.h"
#include "ast/TestFunction.h"
#include "ast/TestApplication.h"

#include "type/TestTypeAndNilType.h"
#include "type/TestIntType.h"
#include "type/TestBoolType.h"
#include "type/TestFunctionType.h"

#include "front/TestToken.h"
#include "front/TestLexer.h"
#include "front/TestParser.h"

#include "kernel/TestUnopPrimitives.h"
#include "kernel/TestBinopPrimitives.h"

#include "core/TestFirstPhase.h"
#include "core/TestBasics.h"

/*
    This is a super basic funclet, which simply saves
    on from having to type out so much code in every
    Test...() body

    since it takes a bool, and then does something with it,
    and then simply returns that same bool, is this procedure
    a monad? or would the bool have to be held in a struct
    for that to be true?
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

/*
	RunTests takes a flags word which indicates what Tests 
	to run, Tests are defined as constexprs which represent 
	different bit-positions in the size_t.
	
	you can check if a flag is set by a bitwise and. 
	and you can set multiple bits simultaneously by bitwise or.
*/
size_t RunTests(std::ostream& out, size_t tests_to_run)
{
    size_t result = 0;

    /*
        Auxilliary Unit Tests.
    */
    if ((tests_to_run & TEST_ERROR) != 0)
        if (TestError(out))
            result |= TEST_ERROR;

    if ((tests_to_run & TEST_OUTCOME) != 0)
        if (TestOutcome(out))
            result |= TEST_OUTCOME;

    if ((tests_to_run & TEST_STRING_INTERNER) != 0)
        if (TestStringInterner(out))
            result |= TEST_STRING_INTERNER;

    if ((tests_to_run & TEST_SYMBOL_TABLE) != 0)
        if (TestSymbolTable(out))
            result |= TEST_SYMBOL_TABLE;

    if ((tests_to_run & TEST_TYPE_INTERNER) != 0)
        if (TestTypeInterner(out))
            result |= TEST_TYPE_INTERNER;

    if ((tests_to_run & TEST_ENVIRONMENT) != 0)
        if (TestEnvironment(out))
            result |= TEST_ENVIRONMENT;

    /*
        Operator Unit Tests
    */
    if ((tests_to_run & TEST_UNOP_CODEGEN) != 0)
        if (TestUnopCodegen(out))
            result |= TEST_UNOP_CODEGEN;

    if ((tests_to_run & TEST_UNOP_LITERAL) != 0)
        if (TestUnopLiteral(out))
            result |= TEST_UNOP_LITERAL;

    if ((tests_to_run & TEST_UNOP_TABLE) != 0)
        if (TestUnopTable(out))
            result |= TEST_UNOP_TABLE;

    if ((tests_to_run & TEST_BINOP_CODEGEN) != 0)
        if (TestBinopCodegen(out))
            result |= TEST_BINOP_CODEGEN;

    if ((tests_to_run & TEST_BINOP_LITERAL) != 0)
        if (TestBinopLiteral(out))
            result |= TEST_BINOP_LITERAL;

    if ((tests_to_run & TEST_BINOP_TABLE) != 0)
        if (TestBinopTable(out))
            result |= TEST_BINOP_TABLE;

    /*
        Abstract Syntax Tree Tests
    */
    if ((tests_to_run & TEST_AST_AND_NIL) != 0)
        if (TestAstAndNil(out))
            result |= TEST_AST_AND_NIL;

    if ((tests_to_run & TEST_BOOL) != 0)
        if (TestBool(out))
            result |= TEST_BOOL;

    if ((tests_to_run & TEST_INT) != 0)
        if (TestInt(out))
            result |= TEST_INT;

    if ((tests_to_run & TEST_VARIABLE) != 0)
        if (TestVariable(out))
            result |= TEST_VARIABLE;

    if ((tests_to_run & TEST_BIND) != 0)
        if (TestBind(out))
            result |= TEST_BIND;

    if((tests_to_run & TEST_BINOP) != 0)
        if (TestBinop(out))
            result |= TEST_BINOP;

    if ((tests_to_run & TEST_UNOP) != 0)
        if (TestUnop(out))
            result |= TEST_UNOP;

    if ((tests_to_run & TEST_ASSIGNMENT) != 0)
      if (TestAssignment(out))
        result |= TEST_ASSIGNMENT;
    
    if ((tests_to_run & TEST_BLOCK) != 0)
      if (TestBlock(out))
        result |= TEST_BLOCK;
    
    if ((tests_to_run & TEST_FUNCTION) != 0)
      if (TestFunction(out))
        result |= TEST_FUNCTION;

    if ((tests_to_run & TEST_APPLICATION) != 0)
      if (TestApplication(out))
        result |= TEST_APPLICATION;

    /*
        Type Tests
    */
    if ((tests_to_run & TEST_TYPE_AND_NIL_TYPE) != 0)
        if (TestTypeAndNilType(out))
            result |= TEST_TYPE_AND_NIL_TYPE;

    if ((tests_to_run & TEST_INT_TYPE) != 0)
        if (TestIntType(out))
            result |= TEST_INT_TYPE;

    if ((tests_to_run & TEST_BOOL_TYPE) != 0)
        if (TestBoolType(out))
            result |= TEST_BOOL_TYPE;
    
    if ((tests_to_run & TEST_FUNCTION_TYPE) != 0)
    	if (TestFunctionType(out))
    		result |= TEST_FUNCTION_TYPE;

    /*
        Kernel Tests
    */
    if ((tests_to_run & TEST_UNOP_PRIMITIVES) != 0)
        if (TestUnopPrimitives(out))
            result |= TEST_UNOP_PRIMITIVES;

    if ((tests_to_run & TEST_BINOP_PRIMITIVES) != 0)
        if (TestBinopPrimitives(out))
            result |= TEST_BINOP_PRIMITIVES;

    /*
        Frontend Tests
    */
    if ((tests_to_run & TEST_TOKEN) != 0)
        if (TestToken(out))
            result |= TEST_TOKEN;

    if ((tests_to_run & TEST_LEXER) != 0)
        if (TestLexer(out))
            result |= TEST_LEXER;
    
    if ((tests_to_run & TEST_PARSER) != 0)
    	if (TestParser(out))
    		result |= TEST_PARSER;

    
   
    /*
     *  Core functionality tests
     */ 
    if ((tests_to_run & TEST_FIRST_PHASE) != 0)
        if (TestFirstPhase(out))
            result |= TEST_FIRST_PHASE;

    if ((tests_to_run & TEST_BASICS) != 0)
        if (TestBasics(out))
            result |= TEST_BASICS;


    return result;
}



void PrintPassedTests(std::ostream& out, size_t tests_run, size_t test_results)
{
    bool result = true;

    /*
        Auxilliary Classes Tests
    */
    if ((tests_run & TEST_ERROR) != 0)           result &= Test(out, "pink::Error",          test_results & TEST_ERROR);
    if ((tests_run & TEST_OUTCOME) != 0)         result &= Test(out, "pink::Outcome",        test_results & TEST_OUTCOME);
    if ((tests_run & TEST_STRING_INTERNER) != 0) result &= Test(out, "pink::StringInterner", test_results & TEST_STRING_INTERNER);
    if ((tests_run & TEST_SYMBOL_TABLE) != 0)    result &= Test(out, "pink::SymbolTable",    test_results & TEST_SYMBOL_TABLE);
    if ((tests_run & TEST_TYPE_INTERNER) != 0)   result &= Test(out, "pink::TypeInterner",   test_results & TEST_TYPE_INTERNER);
    if ((tests_run & TEST_ENVIRONMENT) != 0)     result &= Test(out, "pink::Environment",    test_results & TEST_ENVIRONMENT);
    if ((tests_run & TEST_UNOP_CODEGEN) != 0)    result &= Test(out, "pink::UnopCodegen",    test_results & TEST_UNOP_CODEGEN);
    if ((tests_run & TEST_UNOP_LITERAL) != 0)    result &= Test(out, "pink::UnopLiteral",    test_results & TEST_UNOP_LITERAL);
    if ((tests_run & TEST_UNOP_TABLE) != 0)      result &= Test(out, "pink::UnopTable",      test_results & TEST_UNOP_TABLE);
    if ((tests_run & TEST_BINOP_CODEGEN) != 0)   result &= Test(out, "pink::BinopCodegen",   test_results & TEST_BINOP_CODEGEN);
    if ((tests_run & TEST_BINOP_LITERAL) != 0)   result &= Test(out, "pink::BinopLiteral",   test_results & TEST_BINOP_LITERAL);
    if ((tests_run & TEST_BINOP_TABLE) != 0)     result &= Test(out, "pink::BinopTable",     test_results & TEST_BINOP_TABLE);
    /*
        Abstract Syntax Tree Tests
    */
    if ((tests_run & TEST_AST_AND_NIL) != 0) result &= Test(out, "pink::Ast, pink::Nil", test_results & TEST_AST_AND_NIL);
    if ((tests_run & TEST_BOOL) != 0)        result &= Test(out, "pink::Bool",           test_results & TEST_BOOL);
    if ((tests_run & TEST_INT) != 0)         result &= Test(out, "pink::Int",            test_results & TEST_INT);
    if ((tests_run & TEST_VARIABLE) != 0)    result &= Test(out, "pink::Variable",       test_results & TEST_VARIABLE);
    if ((tests_run & TEST_BIND) != 0)        result &= Test(out, "pink::Bind",           test_results & TEST_BIND);
    if ((tests_run & TEST_BINOP) != 0)       result &= Test(out, "pink::Binop",          test_results & TEST_BINOP);
    if ((tests_run & TEST_UNOP) != 0)        result &= Test(out, "pink::Unop",           test_results & TEST_UNOP);
    if ((tests_run & TEST_ASSIGNMENT) != 0)  result &= Test(out, "pink::Assignment",     test_results & TEST_ASSIGNMENT);
    if ((tests_run & TEST_BLOCK) != 0)       result &= Test(out, "pink::Block",			 test_results & TEST_BLOCK);
    if ((tests_run & TEST_FUNCTION) != 0)    result &= Test(out, "pink::Function",		 test_results & TEST_FUNCTION);
    if ((tests_run & TEST_APPLICATION) != 0) result &= Test(out, "pink::Application",    test_results & TEST_APPLICATION);

    /*
        Type Tests
    */
    if ((tests_run & TEST_TYPE_AND_NIL_TYPE) != 0) result &= Test(out, "pink::Type, pink::NilType", test_results & TEST_TYPE_AND_NIL_TYPE);
    if ((tests_run & TEST_INT_TYPE) != 0)          result &= Test(out, "pink::IntType",             test_results & TEST_INT_TYPE);
    if ((tests_run & TEST_BOOL_TYPE) != 0)         result &= Test(out, "pink::BoolType",            test_results & TEST_BOOL_TYPE);
    if ((tests_run & TEST_FUNCTION_TYPE) != 0)     result &= Test(out, "pink::FunctionType",		test_results & TEST_FUNCTION_TYPE);

    /*
        Frontend Tests
    */
    if ((tests_run & TEST_TOKEN) != 0)  result &= Test(out, "pink::Token",  test_results & TEST_TOKEN);
    if ((tests_run & TEST_LEXER) != 0)  result &= Test(out, "pink::Lexer",  test_results & TEST_LEXER);
    if ((tests_run & TEST_PARSER) != 0) result &= Test(out, "pink::Parser", test_results & TEST_PARSER);

    /*
        Kernel Tests
    */
    if ((tests_run & TEST_UNOP_PRIMITIVES) != 0)  result &= Test(out, "pink::UnopPrimitives",  test_results & TEST_UNOP_PRIMITIVES);
    if ((tests_run & TEST_BINOP_PRIMITIVES) != 0) result &= Test(out, "pink::BinopPrimitives", test_results & TEST_BINOP_PRIMITIVES);
    
    /*
     * Core functionality tests
     */
    if ((tests_run & TEST_FIRST_PHASE) != 0) result &= Test(out, "pink First Phase",              test_results & TEST_FIRST_PHASE);
    if ((tests_run & TEST_BASICS) != 0)      result &= Test(out, "pink Basic core Functionality", test_results & TEST_BASICS);

    Test(out, "pink::Test", result);
}
