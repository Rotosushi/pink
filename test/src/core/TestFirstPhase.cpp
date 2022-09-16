#include <sstream>

#include "Test.h"
#include "core/TestFirstPhase.h"
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
#include "ast/While.h"
#include "ast/Tuple.h"
#include "ast/Dot.h"


bool TestFirstPhase(std::ostream& out)
{
	bool result = true;
  out << "\n-----------------------\n";
  out << "Testing Pink First Phase: \n";
  
  std::stringstream ss;
  ss.str(std::string("nil;\n108;\nfalse;\nx;\ny;\ny := true;\nx := 12;\nz := x;\n")
       + std::string("x = false;\na = false;\n!x;\n!42;\n@12;\nx | !x;\nx + x;\n")
       + std::string("x $ x;\n(x | !x) & (x & !x);\n(x | !x) & (x & 10);\n")
       + std::string("(x | !x) $$ (x & !x);\n100 * 3 == 25 * 12;\n100 * 3 == true * 12;\n")
       + std::string("100 * 3 == 25 $$ 12;\n[0,1,2,3,4];\n[true,false,true,false];\n[0,true];\n")
       + std::string("(0,1,2,3,4);\n(1,true,2,false,3);\nt.0;\nt.1;\nt.5;\n")
       + std::string("if x == true then { 34; } else { 42; };\nif 34 then { true; } else { false; };\n")
       + std::string("if x == true then { true; } else { 10; };\nwhile x == true do { x = false; };\n")
       + std::string("fn one(){1;};\nfn inc(x: Int) { x + 1; };\nfn add(x: Int, y: Int){x + y;};\n")
       + std::string("fn compEq(x: Int, y: Int){x == y;};\nfn addCmp(a: Int, b: Int, c: Int){ x := a + b; x == c; };\n")
       + std::string("fn global(a: Int, b: Int){(a == b) == x;};\n")
       + std::string("one();\none(1);\ninc(1);\ninc(true);\nadd(1,2);\nadd(1,true);\n"));
  auto options = std::make_shared<pink::CLIOptions>();
  auto env     = pink::NewGlobalEnv(options, &ss);

	/*
		Parser emits correct Ast for each Ast node kind,
		and each Ast emits the right type after being returned.
		
		Parser successfully emits errors when it encounters 
		invalid syntax.
		
		Getype successfully emits errors when it encounters 
		untypeable expressions.
	*/
	pink::Ast* term = nullptr;
	pink::Outcome<pink::Type*, pink::Error> getype_result = pink::Error(pink::Error::Code::None, pink::Location());
	
	
	// test parsing and typing a nil literal
	pink::Outcome<std::unique_ptr<pink::Ast>, pink::Error> parser_result = env->parser->Parse(*env);
	
	result &= Test(out, "Parser::Parse nil, Getype() = Nil", 
					   (parser_result) 
				    && ((term  = parser_result.GetOne().get()) != nullptr)
				    && llvm::isa<pink::Nil>(term)
				    && (getype_result = term->Getype(*env))
				    && (getype_result.GetOne() == env->types->GetNilType()));
		
		
	// test parsing and typing a integer literal
	parser_result = env->parser->Parse(*env);
	
	result &= Test(out, "Parser::Parse 108, Getype() = Int", 
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
				   && llvm::isa<pink::Int>(term)
				   && (getype_result = term->Getype(*env))
				   && (getype_result.GetOne() == env->types->GetIntType()));
			
	// test parsing and typing a boolean literal
	parser_result = env->parser->Parse(*env);
	
	result &= Test(out, "Parser::Parse false, Getype() = Bool", 
					 (parser_result) 
				  && ((term  = parser_result.GetOne().get()) != nullptr)
				  && llvm::isa<pink::Bool>(term)
				  && (getype_result = term->Getype(*env))
			    && (getype_result.GetOne() == env->types->GetBoolType()));
	
	// test parsing and typing a bound variable
	pink::InternedString x_var  = env->symbols->Intern("x");
	pink::Type*          x_type = env->types->GetBoolType();
	llvm::Value*         x_val  = env->instruction_builder->getTrue();
	   
	env->bindings->Bind(x_var, x_type, x_val);
	parser_result = env->parser->Parse(*env);
	
	result &= Test(out, "Parser::Parse a Bound Variable, Getype() = Bool", 
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
				   && llvm::isa<pink::Variable>(term)
				   && (getype_result = term->Getype(*env))
				   && (getype_result.GetOne() == env->types->GetBoolType()));
				   

	// test parsing a variable, and typing an unbound variable
	parser_result = env->parser->Parse(*env);
	
	result &= Test(out, "Parser::Parse an Unbound Variable, Getype() = Error", 
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
				   && llvm::isa<pink::Variable>(term)
				   && !(getype_result = term->Getype(*env))
           && (getype_result.GetTwo().code == pink::Error::Code::NameNotBoundInScope));
	
	
	
	// test parsing a bind expression, binding to a new variable
	parser_result = env->parser->Parse(*env);
	
	result &= Test(out, "Parser::Parse a Bind expression for a new Variable, Getype() = Bool", 
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
				   && llvm::isa<pink::Bind>(term)
				   && (getype_result = term->Getype(*env))
				   && (getype_result.GetOne() == env->types->GetBoolType()));
  
  // we must be careful to remember that typing a bind expression constructs 
  // a false binding for that variable, so that we can properly type
  // expressions which follow, and use said variable.


	// test parsing a bind expression, binding to a already bound variable
	parser_result = env->parser->Parse(*env);
	
	result &= Test(out, "Parser::Parse a Bind expression for an existing Variable, Getype() = Error", 
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
				   && llvm::isa<pink::Bind>(term)
				   && !(getype_result = term->Getype(*env))
           && (getype_result.GetTwo().code == pink::Error::Code::NameAlreadyBoundInScope));

    
	parser_result = env->parser->Parse(*env);
	
	result &= Test(out, "Parser::Parse a Bind of a new Variable, reusing an existing binding in the expression, Getype() = Bool",
					   (parser_result)
					&& ((term = parser_result.GetOne().get()) != nullptr)
					&& (getype_result = term->Getype(*env))
					&& (getype_result.GetOne() == env->types->GetBoolType()));
			   
	
	// test parsing an assignment expression, using the definition of 'x' above.
	parser_result = env->parser->Parse(*env);
	
	result &= Test(out, "Parser::Parse an Assignment to an existing Variable, Getype() = Bool", 
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
				   && llvm::isa<pink::Assignment>(term)
				   && (getype_result = term->Getype(*env))
				   && (getype_result.GetOne() == env->types->GetBoolType()));
				   			   
	// test parsing an assignment expression, using an unbound variable
	parser_result = env->parser->Parse(*env);
	
	result &= Test(out, "Parser::Parse an Assignment to a nonexistant Variable, Getype() = Error", 
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
				   && llvm::isa<pink::Assignment>(term)
				   && !(getype_result = term->Getype(*env))
           && (getype_result.GetTwo().code == pink::Error::Code::NameNotBoundInScope));
				   
				   
	// test parsing a unop expression which is known, and is provided the correct type
	parser_result = env->parser->Parse(*env);
	
	result &= Test(out, "Parser::Parse a Unary expression, with a valid unop, and a valid argument type, Getype() = Bool", 
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
				   && llvm::isa<pink::Unop>(term)
				   && (getype_result = term->Getype(*env))
				   && (getype_result.GetOne() == env->types->GetBoolType()));
           
	// test parsing a unop expression which is known, and is provided the incorrect type
	parser_result = env->parser->Parse(*env);
	
	result &= Test(out, "Parser::Parse a Unary expression, with a valid unop and an invalid argument type, Getype() = Error", 
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
				   && llvm::isa<pink::Unop>(term)
				   && !(getype_result = term->Getype(*env))
           && (getype_result.GetTwo().code == pink::Error::Code::ArgTypeMismatch));

	// test parsing a unop expression which is unknown
	parser_result = env->parser->Parse(*env);
	
	result &= Test(out, "Parser::Parse a Unary expression, with an invalid unop, Getype() = Error", 
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
				   && llvm::isa<pink::Unop>(term)
				   && !(getype_result = term->Getype(*env))
           && (getype_result.GetTwo().code == pink::Error::Code::UnknownUnop));
				   
				   
	// test parsing a binop expression which is known, and is provided the correct types
	parser_result = env->parser->Parse(*env); // a tautology
	
	result &= Test(out, "Parser::Parse a Binary expression which is known and provided valid argument types, Getype() = Bool", 
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
				   && llvm::isa<pink::Binop>(term)
				   && (getype_result = term->Getype(*env))
				   && (getype_result.GetOne() == env->types->GetBoolType()));
				   
				   
	// test parsing a binop expression which is known, and is provided the incorrect types
	parser_result = env->parser->Parse(*env);
	
	result &= Test(out, "Parser::Parse a Binary expression which is known and provided the incorrect types, Getype() = Error", 
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
				   && llvm::isa<pink::Binop>(term)
				   && !(getype_result = term->Getype(*env))
           && (getype_result.GetTwo().code == pink::Error::Code::ArgTypeMismatch));
				   
				   
	// test parsing a binop expression which is unknown, and is provided the incorrect types
	parser_result = env->parser->Parse(*env);
	
	result &= Test(out, "Parser::Parse a Binary expression which is unknown, Getype() = Error", 
               (parser_result)
            && ((term = parser_result.GetOne().get()) != nullptr)
            && llvm::isa<pink::Binop>(term)
            && !(getype_result = term->Getype(*env))
            && (getype_result.GetTwo().code == pink::Error::Code::UnknownBinop));

  // so, this is an interesting bug right here. because we choose to not even
  // fully parse an unknown binop expression. For what is ostensibly the good 
  // reason of, we do not know the unknown binops precedence and associativity.
  // what that means is that the parser is not set up to properly parse the 
  // next actual term of the language, and is instead left in a position of
  // having halfway parsed the bad binop term. so that when we try and parse 
  // the next term, via the next call to parse, the parser attempts to parse
  // the rest of the bad expression as if it were the next term to be parsed.
  // so, a quick fix for right here, is to add an extra call to parse to 
  // flush out the rest of the bad term. however this is a bad solution,
  // because it relies on the fact that we happen to know the precise contents 
  // of the input source being parsed, so we can know precisely how much to 
  // throw away, and how much to keep. This makes for a very fragile solution 
  // to the given problem. So instead, what i propose is to parse unknown
  // binops just like we parse unknown unops, that is, assume they are fine
  // and construct some term out of them. This pushes the error itself into the 
  // call to Getype, and out of the call to Parse itself. Which would mean that 
  // the parser would fully consume the bad binary expression, and construct
  // a term of the language out of it. that term then cannot be typed, because
  // of an unknown binop. 
  // This however leaves us with a conundrum, what precedence and associativity 
  // do we give to the unknown binop?
  // if we choose some default values, that seems superficially to be fine,
  // however it raises a concern; namely what about use-before-definition?
  // what if we parsed the binop term, when we did not know it's true
  // precedence and associativity, and then later come accross a definition of 
  // the binop used. This puts us in the bad situation of possibly having
  // guessed wrong, and thus the term we constructed might be incongruent with
  // the definition of the binop itself, and thus the expectations of the user 
  // of the language. 
  //
  // What we would then need is some way to reparse the term which uses the
  // binop, at the time when we encounter it's definition. so that we can
  // construct the correct term with respect to the definition.
  // #USE_BEFORE_DEFINITION

	// test parsing a complex binop expression which is known, and is provided the correct types
	parser_result = env->parser->Parse(*env); // a contradiction
	
	result &= Test(out, "Parser::Parse a complex Binary expression, whose binops are known, and provided the correct types, Getype() = Bool", 
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
				   && llvm::isa<pink::Binop>(term)
				   && (getype_result = term->Getype(*env))
				   && (getype_result.GetOne() == env->types->GetBoolType()));
				   
				   
	// test parsing a complex binop expression which is known, and is provided the incorrect types
	parser_result = env->parser->Parse(*env);
	
	result &= Test(out, "Parser::Parse a complex Binary expression whose binops are known, and provided the incorrect types, Getype() = Error",
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
				   && llvm::isa<pink::Binop>(term)
				   && !(getype_result = term->Getype(*env))
           && (getype_result.GetTwo().code == pink::Error::Code::ArgTypeMismatch));
				   
	// test parsing a complex binop expression which is unknown, and is provided the incorrect types
	parser_result = env->parser->Parse(*env);
	
	result &= Test(out, "Parser::Parse a complex Binary expression whose binops are not all known, Getype() = Error",
	             (parser_result)
            && ((term = parser_result.GetOne().get()) != nullptr)
            && llvm::isa<pink::Binop>(term)
            && !(getype_result = term->Getype(*env))
            && (getype_result.GetTwo().code == pink::Error::Code::UnknownBinop));
	
	// test parsing a mixed-type complex binop expression which is known, and is provided the correct types
	parser_result = env->parser->Parse(*env);
	
	result &= Test(out, "Parser::Parse a mixed-type complex Binary expression whose binops are all known, and are provided the correct types, Getype() = Bool", 
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
				   && llvm::isa<pink::Binop>(term)
				   && (getype_result = term->Getype(*env))
				   && (getype_result.GetOne() == env->types->GetBoolType()));
				   
	// test parsing a mixed-type complex binop expression which is known, and is provided the incorrect types
	parser_result = env->parser->Parse(*env);
	
	result &= Test(out, "Parser::Parse a mixed-type complex Binary expression whose binops are all know, and are provided the incorrect types, Getype() = Error", 
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
				   && llvm::isa<pink::Binop>(term)
				   && !(getype_result = term->Getype(*env))
           && (getype_result.GetTwo().code == pink::Error::Code::ArgTypeMismatch));
				   
	// test parsing a complex binop expression which is unknown, and is provided the incorrect types
	parser_result = env->parser->Parse(*env);
	
	result &= Test(out, "Parser::Parse a mixed-type complex Binary expression whose binops are not all known, Getype() = Error", 
               (parser_result)
            && ((term = parser_result.GetOne().get()) != nullptr)
            && llvm::isa<pink::Binop>(term)
            && !(getype_result = term->Getype(*env))
            && (getype_result.GetTwo().code == pink::Error::Code::UnknownBinop));
		
	
	pink::Type* nil_t  = env->types->GetNilType();
	pink::Type* int_t  = env->types->GetIntType();

  pink::Type* int_5_array_t = env->types->GetArrayType(5, int_t);

  // test parsing and typing an array literal.
  parser_result = env->parser->Parse(*env);

  result &= Test(out, "Parser::Parse an array of integers, Getype() = [ Int x 5 ]",
                (parser_result)
             && ((term = parser_result.GetOne().get()) != nullptr)
             && llvm::isa<pink::Array>(term)
             && (getype_result = term->Getype(*env))
             && (int_5_array_t == getype_result.GetOne()));

	pink::Type* bool_t = env->types->GetBoolType();

  pink::Type* bool_4_array_t = env->types->GetArrayType(4, bool_t);

  parser_result = env->parser->Parse(*env);

  result &= Test(out, "Parser::Parse an array of booleans, Getype() = [ Bool x 4 ]",
              (parser_result)
           && ((term = parser_result.GetOne().get()) != nullptr)
           && llvm::isa<pink::Array>(term)
           && (getype_result = term->Getype(*env))
           && (bool_4_array_t == getype_result.GetOne()));

  parser_result = env->parser->Parse(*env);

  result &= Test(out, "Parser::Parse an array of Int and Bool, Getype() = Error",
                (parser_result)
             && ((term = parser_result.GetOne().get()) != nullptr)
             && llvm::isa<pink::Array>(term)
             && !(getype_result = term->Getype(*env))
             && (getype_result.GetTwo().code == pink::Error::Code::ArrayMemberTypeMismatch));

  std::vector<pink::Type*> tuple_5_int_members = {int_t, int_t, int_t, int_t, int_t};
  pink::Type* tuple_5_int_t = env->types->GetTupleType(tuple_5_int_members);

  parser_result = env->parser->Parse(*env);

  result &= Test(out, "Parser::Parse a Tuple of all Int, Getype() = (Int,Int,Int,Int,Int)",
                (parser_result)
             && ((term = parser_result.GetOne().get()) != nullptr)
             && llvm::isa<pink::Tuple>(term)
             && (getype_result = term->Getype(*env))
             && (tuple_5_int_t == getype_result.GetOne())); 

  std::vector<pink::Type*> tuple_5_int_bool_members = {int_t,bool_t,int_t,bool_t,int_t};
  pink::Type* tuple_5_int_bool_t = env->types->GetTupleType(tuple_5_int_bool_members);

  parser_result = env->parser->Parse(*env);

  result &= Test(out, "Parser::Parse a tuple of Int and Bool, Getype() = (Int,Bool,Int,Bool,Int)",
                (parser_result)
             && ((term = parser_result.GetOne().get()) != nullptr)
             && llvm::isa<pink::Tuple>(term)
             && (getype_result = term->Getype(*env))
             && (tuple_5_int_bool_t == getype_result.GetOne()));

  // 'bind' a variable to a tuple, for typing the dot operator
  pink::InternedString t_var = env->symbols->Intern("t");
  std::vector<pink::Type*> member_types = {int_t, bool_t, int_t, bool_t};
  pink::Type* t_type = env->types->GetTupleType(member_types);
  
  env->bindings->Bind(t_var, t_type, nullptr);
  parser_result = env->parser->Parse(*env);

  result &= Test(out, "Parser::Parse dot operator (t.0) on tuple (Int,Bool,Int,Bool), Getype() = Int",
                (parser_result)
             && ((term = parser_result.GetOne().get()) != nullptr)
             && llvm::isa<pink::Dot>(term)
             && (getype_result = term->Getype(*env))
             && (int_t == getype_result.GetOne()));
  
  parser_result = env->parser->Parse(*env);

  result &= Test(out, "Parser::Parse dot operator (t.1) on tuple (Int,Bool,Int,Bool), Getype() = Bool",
                (parser_result)
             && ((term = parser_result.GetOne().get()) != nullptr)
             && llvm::isa<pink::Dot>(term)
             && (getype_result = term->Getype(*env))
             && (bool_t == getype_result.GetOne()));

  parser_result = env->parser->Parse(*env);

  result &= Test(out, "Parser::Parse dot operator (t.5) on tuple (Int, Bool, Int, Bool), Getype() = Error",
                (parser_result)
             && ((term = parser_result.GetOne().get()) != nullptr)
             && llvm::isa<pink::Dot>(term)
             && !(getype_result = term->Getype(*env))
             && (getype_result.GetTwo().code == pink::Error::Code::DotIndexOutOfRange));

  parser_result = env->parser->Parse(*env);

  result &= Test(out, "Parser::Parse a Conditional, Getype() = Int",
                 (parser_result)
              && ((term = parser_result.GetOne().get()) != nullptr)
              && llvm::isa<pink::Conditional>(term)
              && (getype_result = term->Getype(*env))
              && (getype_result.GetOne() == int_t));

  parser_result = env->parser->Parse(*env);

  result &= Test(out, "Parser::Parse a Conditional, Bad test type, Getype() = Error",
                 (parser_result)
              && ((term = parser_result.GetOne().get()) != nullptr)
              && llvm::isa<pink::Conditional>(term)
              && !(getype_result = term->Getype(*env))
              && (getype_result.GetTwo().code == pink::Error::Code::CondTestExprTypeMismatch));

  parser_result = env->parser->Parse(*env);

  result &= Test(out, "Parser::Parse a Conditional, Mismatched body types, Getype() = Error",
                 (parser_result)
              && ((term = parser_result.GetOne().get()) != nullptr)
              && llvm::isa<pink::Conditional>(term)
              && !(getype_result = term->Getype(*env))
              && (getype_result.GetTwo().code == pink::Error::Code::CondBodyExprTypeMismatch));

  parser_result = env->parser->Parse(*env);

  result &= Test(out, "Parser::Parse a While loop, Getype = Nil",
              (parser_result)
           && ((term = parser_result.GetOne().get()) != nullptr)
           && llvm::isa<pink::While>(term)
           && (getype_result = term->Getype(*env))
           && (nil_t == getype_result.GetOne()));


	std::vector<pink::Type*> args_t;
	pink::Type* simplest_fn_t = env->types->GetFunctionType(int_t, args_t);
	parser_result = env->parser->Parse(*env);
	
	result &= Test(out, "Parser::Parse a Function, no-arg, Getype() = Nil -> Int",
					   (parser_result)
					&& ((term  = parser_result.GetOne().get()) != nullptr)
					&& (llvm::isa<pink::Function>(term))
					&& (getype_result = term->Getype(*env)) // pink::Outcome<>::operator bool 
					&& (getype_result.GetOne() == simplest_fn_t));



  args_t = {int_t};
  pink::Type* inc_fn_t = env->types->GetFunctionType(int_t, args_t);
  parser_result = env->parser->Parse(*env);

  result &= Test(out, "Parser::Parse a Function, one arg, Getype() = Int -> Int",
              (parser_result)
           && ((term = parser_result.GetOne().get()) != nullptr)
           && (llvm::isa<pink::Function>(term))
           && (getype_result = term->Getype(*env))
           && (getype_result.GetOne() == inc_fn_t));
					
	args_t = {int_t, int_t};
	pink::Type* basic_fn_t = env->types->GetFunctionType(int_t, args_t);
	parser_result = env->parser->Parse(*env);
	
	result &= Test(out, "Parser::Parse a Function, two arg, same result type, Getype() = Int -> Int -> Int",
					   (parser_result)
					&& ((term  = parser_result.GetOne().get()) != nullptr)
					&& (llvm::isa<pink::Function>(term))
					&& (getype_result = term->Getype(*env)) // pink::Outcome<>::operator bool
					&& (getype_result.GetOne() == basic_fn_t));

	//args_t = {int_t, int_t};
	pink::Type* comparison_fn_t = env->types->GetFunctionType(bool_t, args_t);
	parser_result = env->parser->Parse(*env);
	
	result &= Test(out, "Parser::Parse a Function, two arg, different result type, Getpe() = Int -> Int -> Bool",
					   (parser_result)
					&& ((term  = parser_result.GetOne().get()) != nullptr)
					&& (llvm::isa<pink::Function>(term))
					&& (getype_result = term->Getype(*env)) // pink::Outcome<>::operator bool 
					&& (getype_result.GetOne() == comparison_fn_t));
					
	args_t = {int_t, int_t, int_t};
	pink::Type* complex_fn_t = env->types->GetFunctionType(bool_t, args_t);
	parser_result = env->parser->Parse(*env);

	result &= Test(out, "Parser::Parse a Function, three arg, different result type, mutiple lines, local binding, Getype() = Int -> Int -> Int -> Bool",
					   (parser_result)
					&& ((term  = parser_result.GetOne().get()) != nullptr)
					&& (llvm::isa<pink::Function>(term))
					&& (getype_result = term->Getype(*env)) // pink::Outcome<>::operator bool 
					&& (getype_result.GetOne() == complex_fn_t));
					
	args_t = {int_t, int_t};
	pink::Type* global_fn_t = env->types->GetFunctionType(bool_t, args_t);
	parser_result = env->parser->Parse(*env);
	
	result &= Test(out, "Parser::Parse a Function, two arg, different result type, uses global variable, Getype() = Int -> Int -> Bool",
					   (parser_result)
					&& ((term  = parser_result.GetOne().get()) != nullptr)
					&& (llvm::isa<pink::Function>(term))
					&& (getype_result = term->Getype(*env)) // pink::Outcome<>::operator bool 
					&& (getype_result.GetOne() == global_fn_t));

  parser_result = env->parser->Parse(*env);

  result &= Test(out, "Parser::Parse an Application, no arg, Getype() = Int",
              (parser_result)
           && ((term = parser_result.GetOne().get()) != nullptr)
           && (llvm::isa<pink::Application>(term))
           && (getype_result = term->Getype(*env))
           && (getype_result.GetOne() == int_t));

  parser_result = env->parser->Parse(*env);

  result &= Test(out, "Parser::Parse an Application, too many args, Getype() = Error",
               (parser_result)
            && ((term = parser_result.GetOne().get()) != nullptr)
            && (llvm::isa<pink::Application>(term))
            && !(getype_result = term->Getype(*env))
            && (getype_result.GetTwo().code == pink::Error::Code::ArgNumMismatch));


  parser_result = env->parser->Parse(*env);

  result &= Test(out, "Parser::Parse an Application, one arg, good arg type, Getype() = Int",
             (parser_result)
          && ((term = parser_result.GetOne().get()) != nullptr)
          && (llvm::isa<pink::Application>(term))
          && (getype_result = term->Getype(*env))
          && (getype_result.GetOne() == int_t));


  parser_result = env->parser->Parse(*env);

  result &= Test(out, "Parser::Parse an Application, one arg, bad arg type, Getype() = Error",
             (parser_result)
          && ((term = parser_result.GetOne().get()) != nullptr)
          && (llvm::isa<pink::Application>(term))
          && !(getype_result = term->Getype(*env))
          && (getype_result.GetTwo().code == pink::Error::Code::ArgTypeMismatch));


  parser_result = env->parser->Parse(*env);

  result &= Test(out, "Parser::Parse an Application, two arg, good arg types, Getype() = Int",
             (parser_result)
          && ((term = parser_result.GetOne().get()) != nullptr)
          && (llvm::isa<pink::Application>(term))
          && (getype_result = term->Getype(*env))
          && (getype_result.GetOne() == int_t));

  parser_result = env->parser->Parse(*env);

  result &= Test(out, "Parser::Parse an Application, two arg, bad arg type, Getype() = Error",
             (parser_result)
          && ((term = parser_result.GetOne().get()) != nullptr)
          && (llvm::isa<pink::Application>(term))
          && !(getype_result = term->Getype(*env))
          && (getype_result.GetTwo().code == pink::Error::Code::ArgTypeMismatch)); 

	result &= Test(out, "pink First Phase", result);

  out << "\n-----------------------\n";
  return result;
}
