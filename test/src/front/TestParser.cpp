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
   ss.str(
      std::string("var x := 0;\n")
   );

   auto options = std::make_shared<pink::CLIOptions>();
   auto env     = pink::NewGlobalEnv(options, &ss); 
   auto parser  = env->parser;
   auto loc     = pink::Location(1, 0, 1, 11);
   auto outcome = parser->Parse(*env);
   pink::Ast* expr = nullptr;

   result &= Test(
      out, 
      "Parser::Parse(\"var x := 0;\")",
      (outcome) &&
      ((expr = outcome.GetFirst().get()) != nullptr) &&
      (llvm::isa<pink::Bind>(expr)) &&
      (loc == expr->GetLoc())
   );
   

   result &= Test(out, "pink::Parser", result);
   out << "\n-----------------------\n";
   return result;
}



