#include <sstream>

#include "Test.h"
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
   *  method to parse each declaration. Thus, we set up the input 
   *  stream at the beginning of this test 
   */ 
  std::stringstream ss;
  ss.str(std::string("nil;\n")    // line 1
       + std::string("10;\n")     // line 2
       + std::string("true;\n")   // line 3
       + std::string("x;\n")      // line 4
       + std::string("x := 1;\n") // line 5
       + std::string("x = 2;\n")  // line 6
       + std::string("!true;\n")  // line 7
       + std::string("1 + 1;\n")  // line 8
       + std::string("6 + 3 * 4 == 3 * 2 + 12;\n") // line 9
       + std::string("(1 + 1) * (1 + 1);\n") // line 10
       + std::string("[0, 1, 2, 3, 4];\n") // line 11
       + std::string("(0,1,2,3,4);\nx.2;\n") // line 12
       + std::string("if true then { 12; } else { 24; };\n") // line 13
       + std::string("while x == true do { x = false; };\n") // line 14
       + std::string("one();\n") // line 15
       + std::string("inc(1);\n") // line 16
       + std::string("add(3,4);\n") // line 17
       + std::string("fn one() { 1; };\n") // line 18
       + std::string("fn inc(x: Int) { x + 1; };\n") // line 19
       + std::string("fn add(x: Int, y: Int, z: Int) { x + y + z; };\n") // line 20
       + std::string("fn fun(x: Int, y: Int, z: Int) { a := x + y; a == z; };\n") // line 21
        ); 
  auto options = std::make_shared<pink::CLIOptions>();
  auto env     = pink::NewGlobalEnv(options, &ss); 
  auto parser = env->parser;
    /*
    	Parser Tests
    	
    		Parses each Basic lexeme 
    		Parses Binding and Assignment
    		Parses Unop expressions
    		Parser Binop expressions
    			Parses Basic Binop Expressions (single operator)
    			Parses Complex Binop Expressions 
    				(multiple operators and up/down + down/up precedence changes)
    		Parses Parenthesized expressions
    */       
    
    pink::Outcome<std::unique_ptr<pink::Ast>, pink::Error> parser_result(parser->Parse(*env));
    pink::Ast* term = nullptr;
    pink::Location loc(1, 0, 1, 3);
    
    result &= Test(out, "Parser::Parse(nil)", 
    		(parser_result) 
    	 && ((term  = parser_result.GetOne().get()) != nullptr) 
    	 && (llvm::isa<pink::Nil>(term))
       && (term->GetLoc() == loc));

    loc = {2, 0, 2, 2};
    parser_result = parser->Parse(*env);
    
    result &= Test(out, "Parser::Parse(10)", 
    	   (parser_result) 
    	&& ((term  = parser_result.GetOne().get()) != nullptr) 
    	&& (llvm::isa<pink::Int>(term))
      && (term->GetLoc() == loc));
    
    loc = {3, 0, 3, 4}; 
    parser_result = parser->Parse(*env);
    
    result &= Test(out, "Parser::Parse(true)", 
    	   (parser_result) 
    	&& ((term  = parser_result.GetOne().get()) != nullptr) 
    	&& (llvm::isa<pink::Bool>(term))
      && (term->GetLoc() == loc));
    
    loc = {4, 0, 4, 1};
    parser_result = parser->Parse(*env);
    
    result &= Test(out, "Parser::Parse(x)", 
    	   (parser_result) 
    	&& ((term  = parser_result.GetOne().get()) != nullptr) 
    	&& (llvm::isa<pink::Variable>(term))
      && (term->GetLoc() == loc));
   
    loc = {5, 0, 5, 6}; 
    parser_result = parser->Parse(*env);
    
    result &= Test(out, "Parser::Parse(x := 1)", 
    	   (parser_result) 
    	&& ((term  = parser_result.GetOne().get()) != nullptr) 
    	&& (llvm::isa<pink::Bind>(term))
      && (term->GetLoc() == loc));
    
    loc = {6, 0, 6, 5};
    parser_result = parser->Parse(*env);
    
    result &= Test(out, "Parser::Parse(x = 2)", 
    	   (parser_result) 
    	&& ((term  = parser_result.GetOne().get()) != nullptr) 
    	&& (llvm::isa<pink::Assignment>(term))
      && (term->GetLoc() == loc));
    
    loc = {7, 0, 7, 5};
    parser_result = parser->Parse(*env);
    
    result &= Test(out, "Parser::Parse(!true)", 
    	   (parser_result) 
    	&& ((term  = parser_result.GetOne().get()) != nullptr) 
    	&& (llvm::isa<pink::Unop>(term))
      && (term->GetLoc() == loc));
    
    // when an ast is representing a binop expression the firstColumn 
    // is filled with the firstColumn associated with the operator within
    // the expression.
    loc = {8, 2, 8, 5};
    parser_result = parser->Parse(*env);
    
    result &= Test(out, "Parser::Parse(1 + 1)", 
    	   (parser_result) 
    	&& ((term  = parser_result.GetOne().get()) != nullptr) 
    	&& (llvm::isa<pink::Binop>(term))
      && (term->GetLoc() == loc));
    
    // the binop with the lowest precedence will appear 
    // first in the tree, due to the operator precedence
    // parser, so that is the location that will be returned
    // when we call GetLoc on any term representing a binop 
    // expr. This is opposed to every other expression,
    // which are parsed in LL(1), which is why the firstColumn
    // must be the firstColumn of the lowest precedence operator
    // in the complex binop expression.
    loc = {9, 10, 9, 23};	
    parser_result = parser->Parse(*env);
    
    result &= Test(out, "Parser::Parse(6 + 3 * 4 == 3 * 2 + 12)",
    	   (parser_result) 
    	&& ((term  = parser_result.GetOne().get()) != nullptr) 
    	&& (llvm::isa<pink::Binop>(term))
      && (term->GetLoc() == loc));
    
    loc = {10, 8, 10, 17};
    parser_result = parser->Parse(*env);
    
    result &= Test(out, "Parser::Parse((1 + 1) * (1 + 1))", 
    	   (parser_result) 
    	&& ((term  = parser_result.GetOne().get()) != nullptr) 
    	&& (llvm::isa<pink::Binop>(term))
      && (term->GetLoc() == loc));  

    loc = {11, 0, 11, 15};
    parser_result = parser->Parse(*env);

    result &= Test(out, "Parser::Parse([0, 1, 2, 3, 4])",
          (parser_result)
       && ((term = parser_result.GetOne().get()) != nullptr)
       && (llvm::isa<pink::Array>(term))
       && (term->GetLoc() == loc));

    loc = {12, 0, 12, 11};
    parser_result = parser->Parse(*env);

    result &= Test(out, "Parser::Parse((0,1,2,3,4))",
              (parser_result)
           && ((term = parser_result.GetOne().get()) != nullptr)
           && (llvm::isa<pink::Tuple>(term)));
    
    loc = {13, 0, 13, 3};
    parser_result = parser->Parse(*env);

    result &= Test(out, "Parser::Parse(x.2)",
              (parser_result)
           && ((term = parser_result.GetOne().get()) != nullptr)
           && (llvm::isa<pink::Dot>(term)));

    loc = {14, 0, 14, 33};
    parser_result = parser->Parse(*env);

    result &= Test(out, "Parser::Parse(if true then { 12; } else { 24; })",
             (parser_result)
          && ((term = parser_result.GetOne().get()) != nullptr)
          && (llvm::isa<pink::Conditional>(term))
          && (term->GetLoc() == loc)); 

    loc = {15, 0, 15, 33};
    parser_result = parser->Parse(*env);

    result &= Test(out, "Parser::Parse(while x == true do { x = false; })",
              (parser_result)
           && ((term = parser_result.GetOne().get()) != nullptr)
           && (llvm::isa<pink::While>(term)));

    loc = {16, 0, 16, 5};
    parser_result = parser->Parse(*env);

    result &= Test(out, "Parser::Parse(one())",
         (parser_result)
      && ((term = parser_result.GetOne().get()) != nullptr)
      && (llvm::isa<pink::Application>(term))); 

    loc = {17, 0, 17, 6};
    parser_result = parser->Parse(*env);

    result &= Test(out, "Parser::Parse(inc(1))",
          (parser_result)
       && ((term = parser_result.GetOne().get()) != nullptr)
       && (llvm::isa<pink::Application>(term)));

    loc = {18, 0, 18, 8}; 
    parser_result = parser->Parse(*env);

    result &= Test(out, "Parser::Parse(add(3,4))",
          (parser_result)
       && ((term = parser_result.GetOne().get()) != nullptr)
       && (llvm::isa<pink::Application>(term)));

    loc = {19, 0, 19, 16};
    parser_result = parser->Parse(*env);
    
    result &= Test(out, "Parser::Parse(fn one() { 1; };)",
    	   (parser_result)
    	&& ((term = parser_result.GetOne().get()) != nullptr)
    	&& (llvm::isa<pink::Function>(term)));
    	
    loc = {20, 0, 20, 26};
    parser_result = parser->Parse(*env);
    
    result &= Test(out, "Parser::Parse(fn inc(x: Int) { x + 1; };)",
    	   (parser_result)
    	&& ((term = parser_result.GetOne().get()) != nullptr)
    	&& (llvm::isa<pink::Function>(term)));
    
    loc = {21, 0, 21, 46};	
    parser_result = parser->Parse(*env);
    
    result &= Test(out, "Parser::Parse(fn add(x: Int, y: Int, z: Int) { x + y + z; };)",
    	   (parser_result)
    	&& ((term = parser_result.GetOne().get()) != nullptr)
    	&& (llvm::isa<pink::Function>(term)));
  
    loc = {22, 0, 22, 56};	
    parser_result = parser->Parse(*env);
    
    result &= Test(out, "Parser::Parse(fn fun(x: Int, y: Int, z: Int) { a := x + y; a == z; };)",
    	   (parser_result)
    	&& ((term = parser_result.GetOne().get()) != nullptr)
    	&& (llvm::isa<pink::Function>(term)));

    result &= Test(out, "pink::Parser", result);
    out << "\n-----------------------\n";
    return result;
}



