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


bool TestParser(std::ostream& out)
{
	bool result = true;
    out << "\n-----------------------\n";
    out << "Testing Pink::Parser: \n";
    
  /*
   *  so, the parser itself handles extracting each line and 
   *  then parsing each one. allowing the calling code to simply 
   *  set up the input_stream, and then repeatedly call the Parse 
   *  method to parse each declaration. but what still hasn't been solved 
   *  is multiple lines for a single declaration.
   *  as of right now, the parser will fail to parse a declaration split over
   *  multiple lines. what we need to do to solve this is to allow each point
   *  within the parser that could potentially need more input than a single 
   *  token to have the ability to ask for more input to finish the parse.
   *  then should we be in the situation where we have partially parsed 
   *  some declaration and we run into the end of the input, that part of 
   *  the parser can ask the input stream for more input and then continue 
   *  trying to parse. from that point, if there is no more input, then we 
   *  have an unfinished declaration, or if the new source is incorrect we
   *  still have an error.
   *
   */ 
  std::stringstream ss;
  ss.str(std::string("nil;\n10;\ntrue;\nx;\nx := 1;\nx = 2;\n!true;\n")
       + std::string("1 + 1;\n6 + 3 * 4 == 3 * 2 + 12;\n")
       + std::string("(1 + 1) - (1 + 1);\nfn one() { 1; };\n")
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
    
    result &= Test(out, "Parser::Parse(unary expression)", 
    	   (parser_result) 
    	&& ((term  = parser_result.GetOne().get()) != nullptr) 
    	&& (llvm::isa<pink::Unop>(term)));
    
    parser_result = parser->Parse(env);
    
    result &= Test(out, "Parser::Parse(binary expression)", 
    	   (parser_result) 
    	&& ((term  = parser_result.GetOne().get()) != nullptr) 
    	&& (llvm::isa<pink::Binop>(term)));
    	
    parser_result = parser->Parse(env);
    
    result &= Test(out, "Parser::Parse(complex binary expression)",
    	   (parser_result) 
    	&& ((term  = parser_result.GetOne().get()) != nullptr) 
    	&& (llvm::isa<pink::Binop>(term)));
    
    parser_result = parser->Parse(env);
    
    result &= Test(out, "Parser::Parse(parenthesized expression)", 
    	   (parser_result) 
    	&& ((term  = parser_result.GetOne().get()) != nullptr) 
    	&& (llvm::isa<pink::Binop>(term)));    

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



