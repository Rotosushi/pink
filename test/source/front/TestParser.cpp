#include <sstream>

#include "Test.h"
#include "support/Common.h"

#include "front/TestParser.h"
#include "front/Parser.h"

#include "aux/Environment.h"

#include "ast/Nil.h"
#include "ast/Int.h"
#include "ast/Bool.h"
#include "ast/Variable.h"
#include "ast/Assignment.h"
#include "ast/Bind.h"
#include "ast/Binop.h"
#include "ast/Unop.h"
#include "ast/Block.h"
#include "ast/Function.h"
#include "ast/Application.h"
#include "ast/Array.h"
#include "ast/Tuple.h"
#include "ast/Conditional.h"
#include "ast/While.h"
#include "ast/Dot.h"





bool TestParser(std::ostream& out)
{
	bool result = true;
   out << "\n-----------------------\n";
   out << "Testing Pink::Parser: \n";
   
  /*
   *  so, the parser itself handles extracting each line and 
   *  then parsing each one. allowing the calling code to simply 
   *  set up the input_stream, and then repeatedly call the Parse 
   *  method to parse each expression. Thus, we set up the input 
   *  stream at the beginning of this test 
   * 
   *  This test routine is meant to test the full breadth of parseable terms.
   *  To fully test what the language supports. 
   */ 
   std::stringstream ss;
   ss.str(
      std::string("var x := 0;\n")                   // line 1
    + std::string("var y := true;\n")                // line 2
    + std::string("var z := x;\n")                   // line 3
    + std::string("var a := -1;\n")                  // line 4
    + std::string("var b := [5, 6, 7, 8, 9];\n")     // line 5
    + std::string("var c := (true, true, false);\n") // line 6
    + std::string("var d := a = -2;\n")              // line 7
    + std::string("fn zero() { 0; }\n")              // line 8
    + std::string("fn inc(x: Int) { x + 1; }\n")     // line 9
    + std::string("fn assign() { x = 4; }\n")        // line 10
   );

   auto options = std::make_shared<pink::CLIOptions>();
   auto env     = pink::NewGlobalEnv(options, &ss); 
   auto parser  = env->parser;

   size_t ln = 1; // counts lines, so we can construct locations
                  // relative to the positions of the tests. 
                  // making it much easier to add a new test between
                  // two existing tests.
   auto loc     = pink::Location(ln, 0, ln, 11);
   pink::Outcome<std::unique_ptr<pink::Ast>, pink::Error> outcome = parser->Parse(*env);
   pink::Ast*  expr = nullptr;

   result &= Test(
      out, 
      "Parser::Parse(\"var x := 0;\")",
      ((expr = GetAstOrNull(outcome)) != nullptr) &&
      TestBindTermIsA<pink::Int>(expr, loc)
   );

   ln++;
   loc = {ln, 0, ln, 14};
   outcome = parser->Parse(*env);

   result &= Test(
      out,
      "Parser::Parse(\"var y := true;\")",
      ((expr = GetAstOrNull(outcome)) != nullptr) &&
      TestBindTermIsA<pink::Bool>(expr, loc)
   );

   ln++;
   loc = {ln, 0, ln, 11};
   outcome = parser->Parse(*env);

   result &= Test(
      out,
      "Parser::Parse(\"var z := x\")",
      ((expr = GetAstOrNull(outcome)) != nullptr) &&
      TestBindTermIsA<pink::Variable>(expr, loc)
   );

   ln++;
   loc = {ln, 0, ln, 12};
   outcome = parser->Parse(*env);

   result &= Test(
      out, 
      "Parser::Parse(\"var a := -1;\")",
      ((expr = GetAstOrNull(outcome)) != nullptr) &&
      TestBindTermIsA<pink::Unop>(expr, loc)
   );

   ln++;
   loc = {ln, 0, ln, 25};
   outcome = parser->Parse(*env);

   result &= Test(
      out,
      "Parser::Parse(\"var b := [5, 6, 7, 8, 9];\")",
      ((expr = GetAstOrNull(outcome)) != nullptr) && 
      TestBindTermIsA<pink::Array>(expr, loc)
   );

   ln++;
   loc = {ln, 0, ln, 29};
   outcome = parser->Parse(*env);

   result &= Test(
      out, 
      "Parser::Parse(\"var c := (true, true, false);\")",
      ((expr = GetAstOrNull(outcome)) != nullptr) &&
      TestBindTermIsA<pink::Tuple>(expr, loc)
   );

   ln++;
   loc = {ln, 0, ln, 16};
   outcome = parser->Parse(*env);

   result &= Test(
      out,
      "Parser::Parse(\"var d := a = -2;\")",
      ((expr = GetAstOrNull(outcome)) != nullptr) && 
      TestBindTermIsA<pink::Assignment>(expr, loc)
   );
   


   ln++;
   loc = {ln, 0, ln, 16};
   outcome = parser->Parse(*env);

   result &= Test(
      out,
      "Parser::Parse(\"fn zero() { 0; }\")",
      ((expr = GetAstOrNull(outcome)) != nullptr) &&
      TestFnBodyIsA<pink::Int>(expr, loc)
   );

   ln++;
   loc = {ln, 0, ln, 25};
   outcome = parser->Parse(*env);

   result &= Test(
      out,
      "Parser::Parse(\"fn inc(x: Int) { x + 1; }\")",
      ((expr = GetAstOrNull(outcome)) != nullptr) &&
      TestFnBodyIsA<pink::Binop>(expr, loc)
   );

   ln++;
   loc = {ln, 0, ln, 22};
   outcome = parser->Parse(*env);

   result &= Test(
      out,
      "Parser::Parse(\"fn assign() { x = 4; }\")",
      ((expr = GetAstOrNull(outcome)) != nullptr) &&
      TestFnBodyIsA<pink::Assignment>(expr, loc)
   );

   result &= Test(out, "pink::Parser", result);
   out << "\n-----------------------\n";
   return result;
}



