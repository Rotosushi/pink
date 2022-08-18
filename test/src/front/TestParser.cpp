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
#include "ast/Conditional.h"

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
  ss.str(std::string("nil;\n10;\ntrue;\nx;\nx := 1;\nx = 2;\n!true;\n")
       + std::string("1 + 1;\n6 + 3 * 4 == 3 * 2 + 12;\n")
       + std::string("(1 + 1) * (1 + 1);\n[0, 1, 2, 3, 4];\n")
       + std::string("if true then { 12; } else { 24; };\n")
       + std::string("one();\ninc(1);\nadd(3,4);\nfn one() { 1; };\n")
       + std::string("fn inc(x: Int) { x + 1; };\n")
       + std::string("fn add(x: Int, y: Int, z: Int) { x + y + z; };\n")
       + std::string("fn fun(x: Int, y: Int, z: Int) { a := x + y; a == z; };\n")); 
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
    
    pink::Outcome<std::unique_ptr<pink::Ast>, pink::Error> parser_result(parser->Parse(env));
    pink::Ast* term = nullptr;
    
    result &= Test(out, "Parser::Parse(nil)", 
    		(parser_result) 
    	 && ((term  = parser_result.GetOne().get()) != nullptr) 
    	 && (llvm::isa<pink::Nil>(term)));

    parser_result = parser->Parse(env);
    
    result &= Test(out, "Parser::Parse(10)", 
    	   (parser_result) 
    	&& ((term  = parser_result.GetOne().get()) != nullptr) 
    	&& (llvm::isa<pink::Int>(term)));
     
    parser_result = parser->Parse(env);
    
    result &= Test(out, "Parser::Parse(true)", 
    	   (parser_result) 
    	&& ((term  = parser_result.GetOne().get()) != nullptr) 
    	&& (llvm::isa<pink::Bool>(term)));
    
    parser_result = parser->Parse(env);
    
    result &= Test(out, "Parser::Parse(x)", 
    	   (parser_result) 
    	&& ((term  = parser_result.GetOne().get()) != nullptr) 
    	&& (llvm::isa<pink::Variable>(term)));
    
    parser_result = parser->Parse(env);
    
    result &= Test(out, "Parser::Parse(x := 1)", 
    	   (parser_result) 
    	&& ((term  = parser_result.GetOne().get()) != nullptr) 
    	&& (llvm::isa<pink::Bind>(term)));
    
    parser_result = parser->Parse(env);
    
    result &= Test(out, "Parser::Parse(x = 2)", 
    	   (parser_result) 
    	&& ((term  = parser_result.GetOne().get()) != nullptr) 
    	&& (llvm::isa<pink::Assignment>(term)));
    
    parser_result = parser->Parse(env);
    
    result &= Test(out, "Parser::Parse(!true)", 
    	   (parser_result) 
    	&& ((term  = parser_result.GetOne().get()) != nullptr) 
    	&& (llvm::isa<pink::Unop>(term)));
    
    parser_result = parser->Parse(env);
    
    result &= Test(out, "Parser::Parse(1 + 1)", 
    	   (parser_result) 
    	&& ((term  = parser_result.GetOne().get()) != nullptr) 
    	&& (llvm::isa<pink::Binop>(term)));
    	
    parser_result = parser->Parse(env);
    
    result &= Test(out, "Parser::Parse(6 + 3 * 4 == 3 * 2 + 12)",
    	   (parser_result) 
    	&& ((term  = parser_result.GetOne().get()) != nullptr) 
    	&& (llvm::isa<pink::Binop>(term)));
    
    parser_result = parser->Parse(env);
    
    result &= Test(out, "Parser::Parse((1 + 1) * (1 + 1))", 
    	   (parser_result) 
    	&& ((term  = parser_result.GetOne().get()) != nullptr) 
    	&& (llvm::isa<pink::Binop>(term)));  

    parser_result = parser->Parse(env);

    result &= Test(out, "Parser::Parse([0, 1, 2, 3, 4])",
          (parser_result)
       && ((term = parser_result.GetOne().get()) != nullptr)
       && (llvm::isa<pink::Array>(term)));

    parser_result = parser->Parse(env);

    result &= Test(out, "Parser::Parse(if true then { 12; } else { 24; })",
             (parser_result)
          && ((term = parser_result.GetOne().get()) != nullptr)
          && (llvm::isa<pink::Conditional>(term))); 

    parser_result = parser->Parse(env);

    result &= Test(out, "Parser::Parse(application expression, no arg)",
         (parser_result)
      && ((term = parser_result.GetOne().get()) != nullptr)
      && (llvm::isa<pink::Application>(term))); 

    parser_result = parser->Parse(env);

    result &= Test(out, "Parser::Parse(application expression, one arg)",
          (parser_result)
       && ((term = parser_result.GetOne().get()) != nullptr)
       && (llvm::isa<pink::Application>(term)));

    parser_result = parser->Parse(env);

    result &= Test(out, "Parser::Parse(application expression, multi arg)",
          (parser_result)
       && ((term = parser_result.GetOne().get()) != nullptr)
       && (llvm::isa<pink::Application>(term)));

    parser_result = parser->Parse(env);
    
    result &= Test(out, "Parser::Parse(Function, no arg)",
    	   (parser_result)
    	&& ((term = parser_result.GetOne().get()) != nullptr)
    	&& (llvm::isa<pink::Function>(term)));
    	
    	
    parser_result = parser->Parse(env);
    
    result &= Test(out, "Parser::Parse(Function, single arg)",
    	   (parser_result)
    	&& ((term = parser_result.GetOne().get()) != nullptr)
    	&& (llvm::isa<pink::Function>(term)));
    	
    parser_result = parser->Parse(env);
    
    result &= Test(out, "Parser::Parse(Function, multi-arg)",
    	   (parser_result)
    	&& ((term = parser_result.GetOne().get()) != nullptr)
    	&& (llvm::isa<pink::Function>(term)));
    	
    parser_result = parser->Parse(env);
    
    result &= Test(out, "Parser::Parse(Function, multi-arg, multi-line)",
    	   (parser_result)
    	&& ((term = parser_result.GetOne().get()) != nullptr)
    	&& (llvm::isa<pink::Function>(term)));

    result &= Test(out, "pink::Parser", result);
    out << "\n-----------------------\n";
    return result;
}



