#include "front/Parser.h"

#include "aux/Environment.h"

// Syntactic Forms
#include "ast/Variable.h"
#include "ast/Bind.h"
#include "ast/Assignment.h"
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

// Values
#include "ast/Bool.h"
#include "ast/Int.h"
#include "ast/Nil.h"

// Operators
#include "ast/Unop.h"
#include "ast/Binop.h"

// Types
#include "type/NilType.h"
#include "type/IntType.h"
#include "type/BoolType.h"
#include "type/PointerType.h"
#include "type/ArrayType.h"
#include "type/TupleType.h"

namespace pink {
    Parser::Parser()
      : lexer(), tok(Token::End), loc(1, 0, 1, 0), txt(), input_stream(&std::cin)
    {

    }

    Parser::Parser(std::istream* i)
      : lexer(), tok(Token::End), loc(1, 0, 1, 0), txt(), input_stream(i)
    {
      if (i == nullptr)
        FatalError("istream cannot be nullptr when initializing the Parser. text cannot be extracted from nowhere.", __FILE__, __LINE__);
    }

    Parser::~Parser()
    {

    }

    const std::string& Parser::GetBuf()
    {
      return lexer.GetBuf();
    }

    std::istream* Parser::GetIStream()
    {
      return input_stream;
    }
    
    void Parser::SetIStream(std::istream* stream)
    {
      if (stream == nullptr)
        FatalError("istream cannot be nullptr when setting the input stream of the Parser.", __FILE__, __LINE__);
     
      input_stream = stream;
    }

    bool Parser::EndOfInput()
    {
      return lexer.EndOfInput();     
    }

    void Parser::Getline(std::string& buf)
    {
      char c = '\0';
      
      do {
        c = input_stream->get();
        
        if (input_stream->eof()) // don't append the EOF character
          break;

        buf += c; // appends the character even when it's '\n'

      } while (c != '\n');
    }

    void Parser::yyfill()
    {
      std::string t;
      Getline(t);
      lexer.AppendBuf(t);
    }

    void Parser::nexttok()
    {
      tok = lexer.yylex(); // this statement advances the lexer's internal state.
      loc = lexer.yyloc();
      txt = lexer.yytxt();

      while (tok == Token::End && !input_stream->eof())
      {
        yyfill();
        tok = lexer.yylex();
        loc = lexer.yyloc();
        txt = lexer.yytxt();
      }
    }


    /*  
     *  So, Parse returns a single term when it is called.
     *  how then does our language handle source files which are longer than 
     *  a single line of text? 
     *  well, Parse is designed to work in a 'push' style rather than a 'pull' 
     *  style. that is, instead of expecting all of the source to be parsed 
     *  to be available all at once. the Parser instead has a reference to an
     *  input stream and will ask for more source when it finishes parsing the
     *  single line. in this way, the caller of Parse is expected to repeatedly 
     *  call Parse, to eventually parse all of the source. 
     *  
     *  the grammar that this parser accepts, 
     *  in (mostly) BNF form is:
     *  
     *  #TODO: make this parser match this grammar. (the grammar just changed)
     *
     *  term := affix ';'
     *
     *  affix := function
     *         | definition
     *         | application
     *         | application '=' affix
     *         | 
     *         | application (operator application)* // this isn't actually a simple
     *                                   // kleen star, but actually an entrance 
     *                                   // to an operator precedence parser.
     *                                   // however this is optional, just like
     *                                   // the kleen star is, and I don't know 
     *                                   // how to specify an operator
     *                                   // precedence parser in BNF syntax.
     *                                   // especially given it's a subset of
     *                                   // LR parsable languages. and
     *                                   // otherwise this language is LL(1)
     *  
     *  application := basic
     *               | basic '(' (affix (',' affix)*)? ')'
     *
     *
     *  basic := nil
     *         | true
     *         | false
     *         | integer
     *         | operator basic
     *         | '(' affix ')' 
     *         | id
     *         | '[' (affix (',' affix)*)? ']'
     *         | '[' (affix 'x' affix)? ']'
     *
     *  function := 'fn' id '(' (arg (',' arg)*)? ')' '{' term+ '}'
     *
     *  definition := id ':=' affix
     *
     *  arg := id ':' type
     *
     *  type := basic_type ('*')?
     *
     *  basic_type := 'Nil'
     *              | 'Int'
     *              | 'Bool'
     *              | '[' type 'x' affix ']'
     *
     *



      the gap between the reality of an idea                       and your idea of that idea.

      the gap between the reality of an others idea                and your idea of their idea.
      
      the gap between the reality of an idea                       and your ability to communicate that idea.
      
      the gap between the reality of an idea                       and your ability to understand that idea.
      
      the gap between the reality of what is good about an idea    and your idea of what is good about that idea.
      
      the gap between the reality of what is bad about an idea     and your idea of what is bad about that idea.
      
      the gap between the reality of what is neutral about an idea and your idea of what is neutral about that idea.
      
      the gap between the reality of what is possible              and your idea of what is possible.
      
      the gap between the reality of what is probable              and your idea of what is probable.
      
      the gap between the reality of any object                    and your ability to encapsulate that object within numbers.

      the gap between the reality                                  and your idea of the reality.

     *  
     *  okay, i need to think about this, and I don't really know 
     *  where to put this comment, but where exactly do we place 
     *  calls to CreateLoad instructions? 
     *
     *  we used to simply load the value of a variable when it was 
     *  referenced within the text. which was fine, because we didn't 
     *  have pointers before. 
     *  the reason adding pointers is problematic is because each 
     *  AllocaInst/GlobalVariable is already a pointer. 
     *  and since each value we needed to deal with was the value 
     *  the llvm::pointer pointed too, this was fine.
     *
     *  but the question becomes what about when the value we want  
     *  to retrieve is the llvm::pointer's value itself? 
     *  as in the case of a value representing a pointer to some 
     *  memory. 
     *
     *  i suppose what we want is two implementations of 
     *  Variable::Codegen. one where we load before returning 
     *  the result of the load, which should happen if the type 
     *  of this variable is a pointer, or we should simply 
     *  return the llvm::Value* itself, in the case where
     *  the variable is a pointer itself.
     *
     *  then, addition on pointers could be acheived 
     *  by the same instruction, except we pass in 
     *  the pointers themselves as values instead of a 
     *  loadInst. I guess that means this comment belongs in 
     *  pink::Variable.
     *  
     *  okay, this solution works great for the times when we are 
     *  referencing the variable as it's value.
     *  however, when we want to assign a new value into the 
     *  variable's location, we don't want to have already loaded it,
     *  instead we simply want the ptr referencing the memory again,
     *  like in the previous case.
     *
     *  however, how can we communicate that to Variable::Codegen?
     *
     *  well, the first thing i notice is that this is suspiciously 
     *  like c's lvalues and rvalues in their semantics.
     *  
     *  the value on the left is being assigned, so we want the address,
     *  the value on the right is being used and so we want to load 
     *  it's value into a register.
     *
     *  we might be able to store a bit in the environment? which is 
     *  kinda weird, however, we can set the bit only when we codegen 
     *  an assignment's lhs. 
     *  and due to the way the parser works, nested assignment terms 
     *  will construct tree's like:
     *  
     *  a = b = c = ...;
     *
     *         =
     *       /   \
     *      a      =
     *           /   \
     *          b      =
     *               /   \
     *              c     ...;
     *
     * which means if we unset the bit after codegening the lhs we 
     * will not treat the rhs as a value being assigned, and within 
     * the rhs, we can have another assignment term with it's own lhs
     *
     * so, one thing that comes to mind is that we don't necessarily have
     * only variable terms on the lhs of an assignment,
     *
     * we can imagine terms which look like 
     *
     * x + y = ...;
     *
     * where x is a pointer into an array,
     * and y is an offset.
     *
     * although, that would mean that x would have pointer type, 
     * and thus we could make pointers exempt from this rule.
     * which would mean we didn't load x, however in this situation 
     * we want to load y. and since it appears on the left, this 
     * naive approach would cause us to not load it.
     *
     * since we don't allow assignment to literals,
     * the only case where we would validly assign to a temporary 
     * value is if that value has pointer type.
     *
     * however, we wouldn't want to assign in the case of a term like: 
     * 
     * &var = ...;
     *
     * so, lets recap:
     *
     *  // "//+" means this syntax only makes sense if the temp value produced
     *  //       makes sense to assign to, and otherwise is a semantic error
     *
     *  1) x = ...; // we don't have any concept of const yet
     *  2) x op y = ...; //+
     *  3) op x = ...;   //+
     *  4) x(...) = ...; //+
     *
     *
     *
     *  okay, so what about following C a little bit again, with keeping track 
     *  of the assignable state of each value within the language?
     *
     *  pointers to memory are assignable, that is
     *  llvm::AllocaInst/llvm::GlobalVariable
     *
     *  and regular values, that is any llvm::Value* that does not represent an
     *  allocation of memory which can be assigned into is not assignable.
     *
     *  then, we can modify operators to also do work on this assignable state,
     *  so, when an operation is performed, we can return either an assignable 
     *  or an unassignable.
     *
     *  well, all that is fine, but it still doesn't change the fact that we
     *  have to load a value from a variable reference on the right side of 
     *  assignment, and not load a value from a variable reference on the left 
     *  side of assignment. if we had the variable marked as assignable, we 
     *  would still need different behavior on that assignable variable
     *  depending on if it appeared on the left or the right hand side.
     *
     *  so i suppose we need a condition in Variable::Codegen which 
     *  has the behavior we want depending on if we were on the left 
     *  or the right hand side. but this solution needs to play nice 
     *  with each of the possibilities above. for instance, if we 
     *  were computing an offset into an array, by adding a pointer 
     *  variable together with a integer variable, we would need to not 
     *  load the pointer variable. (which we can handle by way of the fact 
     *  that we treat pointer variables differently to all other variables)
     *  yet we would need to load the regular variable. even though it is on 
     *  the left. We could even imagine a complex arithmetic expression
     *  including multiple values which computes the offset, and any variable 
     *  appearing within that expression would need to be handled as we would 
     *  with any arithmetic expression appearing on the right hand side.
     *
     *  similarly any arithmetic expression appearing within the argument
     *  position of a function call would need to be treated normally.
     *
     *  if we had a pointer to a pointer to some value, we could have an expression like 
     *  
     *  *ptr = ...;
     *
     *  this would require the rhs to be of pointer type, obviously. but it
     *  would also require that we perform a load from the first pointer to 
     *  get the second and then we would be modifying the second ptr. 
     *  inside llvm (ptr) would be a pointer to the memory where the 
     *  first pointer is stored, which would hold a value which is a pointer 
     *  to the second location which is itself another pointer to a third
     *  location. the * operator should retreive the pointer type value
     *  
     *  so i think this works naturally, iff the pointers themselves are 
     *  unassignable, and then the * operator changes their type such that 
     *  they become assignable.  
     *
     *
     *       *
 */
    Outcome<std::unique_ptr<Ast>, Error> Parser::Parse(const Environment& env)
    { 
      if (tok == Token::End && EndOfInput() && !input_stream->eof())
        nexttok(); // prime the lexer with the first token from the input stream

      // if tok == Token::End even after we tried to get more input from the given
      // file it means we read to the EOF, so we return the end of file error, to signal 
      // to the caller that the end of the associated input has been reached.
      if (tok == Token::End)
      {
        return Outcome<std::unique_ptr<Ast>, Error>(Error(Error::Code::EndOfFile, loc)); 
      }

      // Parse a single expression from the source file.
    	return ParseTop(env);	
    }

    /*
      top = function
          | variable
    */
    Outcome<std::unique_ptr<Ast>, Error> Parser::ParseTop(const Environment& env)
    {
      if (tok == Token::Fn)
      {
        return ParseFunction(env);
      }
      else if (tok == Token::Var)
      {
        return ParseVariable(env);
      }
      else // assume the text appearing defines a term other than a function or variable
      {
        if (tok == Token::LBrace)
        {
          auto block = ParseBlock(env);

          if (!block)
            return block.GetSecond();
        }
        else
        {
          // Parse the term, in an attempt to not leave the parser in an unusable state
          auto loop = ParseTerm(env);

          // if we take this branch, the parser is probably still pointing 
          // to somewhere within the text describing the term. leaving it 
          // unable to continue parsing the file as if the next expression 
          // was another top level expression.
          if (!loop)
            return loop.GetSecond();
        }
        
        return Error(Error::Code::BadTopLevelExpression, loc, "only variables and functions may be declared at global scope");
      }
    }

    /*
      variable = "var" id ":=" affix ";"
    */
   Outcome<std::unique_ptr<Ast>, Error> Parser::ParseVariable(const Environment& env)
   {
      Location lhs_loc = loc;
      InternedString symbol = nullptr;

      if (tok != Token::Var)
        return Error(Error::Code::MissingVar, loc, "parsed: " + txt);

      nexttok(); // eat 'var'

      if (tok != Token::Id)
        return Error(Error::Code::MissingBindId, loc, "parsed: " + txt);

      symbol = env.symbols->Intern(txt);

      nexttok();

      if (tok != Token::ColonEq)
        return Error(Error::Code::MissingBindColonEq, loc, "parsed: " + txt);

      nexttok();

      auto affix = ParseAffix(env);

      if (!affix)
        return affix.GetSecond();

      if (tok != Token::Semicolon)
        return Error(Error::Code::MissingSemicolon, loc, "parsed: " + txt);

      nexttok();
      
      Location bind_loc(lhs_loc.firstLine, lhs_loc.firstColumn, loc.firstLine, loc.firstColumn);
      return Outcome<std::unique_ptr<Ast>, Error>(std::make_unique<Bind>(bind_loc, symbol, std::move(affix.GetFirst())));
   }

    /*
      function = "fn" id "(" [arg {"," arg}] ")" block
    */
    Outcome<std::unique_ptr<Ast>, Error> Parser::ParseFunction(const Environment& env)
    {
    	InternedString name = nullptr;
    	std::vector<std::pair<InternedString, Type*>> args;
    	std::unique_ptr<Ast> body(nullptr);
    	Location lhs_loc = loc;
    
      // this does not actually work. we don't touch the bindings within a function 
      // until typechecking. so this check will always pass even 
      // if we are parsing a function definition which occurs inside 
      // another function definition.
    	//  if (env->bindings->OuterScope() != nullptr)
    	//  {
			//    Error error(Error::Kind::Syntax, "Functions can only be defined at the global scope", loc);
			//    return Outcome<std::unique_ptr<Ast>, Error>(error);    	
    	//  }
      //
      //  so we have two options, accept that we catch this particular error at
      //  typechecking, and leave this commented out
      //  or figure out a way to know we are parsing inside of a function
      //  definition at the moment we try to parse a function definition.
    	
    	
    	if (tok != Token::Fn)
    		return Error(Error::Code::MissingFn, loc);
    	
    	nexttok(); // eat 'fn'
    	
    	if (tok != Token::Id)
    		return Error(Error::Code::MissingFnName, loc);
    	
    	name = env.symbols->Intern(txt); // intern the functions name
		
		  nexttok(); // eat 'Id'

		  if (tok != Token::LParen)
        return Error(Error::Code::MissingLParen, loc);
      
      nexttok(); // eat '('

      if (tok == Token::Id) // beginning of the argument list
      {
        do {
          if (tok == Token::Comma)
            nexttok(); // eat ','
        
          Outcome<std::pair<InternedString, Type*>, Error> arg_res = ParseArgument(env);
          
          if (!arg_res)
            return arg_res.GetSecond();
          
          args.emplace_back(arg_res.GetFirst());
        } while (tok == Token::Comma);
      }
      
      // handle the case where we just parsed an argument list, and 
      // the case where we parsed a no argument argument list, either 
      // way the next token must be ')'
      if (tok != Token::RParen)
        return Error(Error::Code::MissingRParen, loc);
      
      nexttok(); // eat ')'
      
      // parse the body of the function.
      auto body_res = ParseBlock(env);
      
      if (!body_res)
        return body_res.GetSecond();
      
      Location fn_loc = {lhs_loc.firstLine, lhs_loc.firstColumn, loc.firstLine, loc.firstColumn};
      
      // we have all the parts, build the function.
      return Outcome<std::unique_ptr<Ast>, Error>(std::make_unique<Function>(fn_loc,
                                                                             name, 
                                                                             args, 
                                                                             std::move(body_res.GetFirst()), 
                                                                             env.bindings.get())
                                                 );		
    }
    
    /*
      arg = id ":" type
    */
    Outcome<std::pair<InternedString, Type*>, Error> Parser::ParseArgument(const Environment& env)
    {
    	InternedString name;

    	if (tok != Token::Id)
    		return Error(Error::Code::MissingArgName, loc);
    	
    	name = env.symbols->Intern(txt);
    	
    	nexttok(); // eat 'Id'

    	if (tok != Token::Colon)
    		Error(Error::Code::MissingArgColon, loc);
    	
    	nexttok(); // eat ':'

      // if (!isTypeToken(tok))
      if (tok == Token::Comma
       || tok == Token::RParen)
      {
        return Error(Error::Code::MissingArgType, loc);
      }

    	Outcome<Type*, Error> type_res = ParseType(env);
    	
    	if (!type_res)
    		return type_res.GetSecond();
    	
    	return Outcome<std::pair<InternedString, Type*>, Error>(std::make_pair(name, type_res.GetFirst()));
    }

    /*
      block = "{" {term} "}"
    */
    Outcome<std::unique_ptr<Ast>, Error> Parser::ParseBlock(const Environment& env)
    {
    	std::vector<std::unique_ptr<Ast>> stmnts;
    	Outcome<std::unique_ptr<Ast>, Error> result(Error(Error::Code::None, loc));
    	Location left_loc = loc;

      if (tok != Token::LBrace)
			  return Error(Error::Code::MissingLBrace, loc);
		
		  nexttok(); // eat '{'
    	
    	do {		
    		// parse a term.
        // we recurr up to ParseTerm which requires the expression we parse to 
        // end in a semicolon, that way, a block is composed of a sequence of 
        // semicolon separated affix expressions.
    		result = ParseTerm(env);
    		
    		if (!result)
    			return result.GetSecond();
    		
    		// add it to the current block
    		stmnts.emplace_back(std::move(result.GetFirst()));
      } while (tok != Token::RBrace);

      if (tok != Token::RBrace)
        return Error(Error::Code::MissingRBrace, loc);

      nexttok(); // eat '}'
    	
    	Location block_loc(left_loc.firstLine, left_loc.firstColumn, loc.firstLine, loc.firstColumn);
    	return Outcome<std::unique_ptr<Ast>, Error>(std::make_unique<Block>(block_loc, stmnts));
    }
    
    /*
      term = conditional
           | while
           | variable
           | affix ";"
    */
    Outcome<std::unique_ptr<Ast>, Error> Parser::ParseTerm(const Environment& env)
    {
      if (tok == Token::If)
      {
        return ParseConditional(env);
      }
      else if (tok == Token::While)
      {
        return ParseWhile(env);
      }
      else if (tok == Token::Var)
      {
        return ParseVariable(env);
      }
      else // assume this is an affix term 
      {
        Outcome<std::unique_ptr<Ast>, Error> affix = ParseAffix(env);

        // if we take this branch we may leave the parser pointing into 
        // the middle of a term
        if (!affix)
          return affix.GetSecond();

        if (tok != Token::Semicolon)
        {
          Error error(Error::Code::MissingSemicolon, loc, "instead parsed " + txt);
          return error;
        }

        nexttok(); // eat ';'

        return Outcome<std::unique_ptr<Ast>, Error>(std::move(affix.GetFirst()));
      }
    }
    
    /*
      conditional = "if" "(" affix ")" block "else" block
    */
    Outcome<std::unique_ptr<Ast>, Error> Parser::ParseConditional(const Environment& env)
    {
      Location lhs_loc = loc;
      
      if (tok != Token::If)
        return Error(Error::Code::MissingIf, loc);

      nexttok(); // eat "if"

      if (tok != Token::LParen)
        return Error(Error::Code::MissingLParen, loc);

      nexttok(); // eat "("
      
      Outcome<std::unique_ptr<Ast>, Error> test_term = ParseAffix(env);

      if (!test_term)
        return test_term.GetSecond();

      if (tok != Token::RParen)
        return Error(Error::Code::MissingRParen, loc);

      nexttok(); // eat ")"   
      
      Outcome<std::unique_ptr<Ast>, Error> then_term = ParseBlock(env);

      if (!then_term)
        return then_term.GetSecond();


      if (tok != Token::Else)
        return Error(Error::Code::MissingElse, loc);

      nexttok(); // eat 'else'

      Outcome<std::unique_ptr<Ast>, Error> else_term = ParseBlock(env);

      if (!else_term)
        return else_term.GetSecond();

      Location condloc(lhs_loc.firstLine, lhs_loc.firstColumn, loc.firstLine, loc.firstColumn);
      return Outcome<std::unique_ptr<Ast>, Error>(std::make_unique<Conditional>(condloc, std::move(test_term.GetFirst()), std::move(then_term.GetFirst()), std::move(else_term.GetFirst())));
    }

    /*
      while = "while" "(" affix ")" block
    */
    Outcome<std::unique_ptr<Ast>, Error> Parser::ParseWhile(const Environment& env)
    {
      Location lhs_loc = loc;
      if (tok != Token::While)
        return Error(Error::Code::MissingWhile, loc);
      
      nexttok(); // eat 'while'

      auto test_term = ParseAffix(env);

      if (!test_term)
        return test_term.GetSecond();

      if (tok != Token::Do)
        return Error(Error::Code::MissingDo, loc);

      nexttok(); // eat 'do'

      auto body_term = ParseBlock(env);

      if (!body_term)
        return body_term.GetSecond();

      Location whileloc(lhs_loc.firstLine, lhs_loc.firstColumn, loc.firstLine, loc.firstColumn);
      return Outcome<std::unique_ptr<Ast>, Error>(std::make_unique<While>(whileloc, std::move(test_term.GetFirst()), std::move(body_term.GetFirst())));
    }

    /*
      affix = composite "=" affix
            | composite "(" [affix {"," affix}] ")"
            | composite
    */
    Outcome<std::unique_ptr<Ast>, Error> Parser::ParseAffix(const Environment& env)
    {
      Location lhs_loc = loc;

      auto composite = ParseComposite(env);

      if (!composite)
        return composite.GetSecond();

      // an assignment expression
      if (tok == Token::Equals)
      {
        nexttok(); // eat "="

        auto rhs = ParseAffix(env);
		        
		    if (!rhs)
		      return rhs.GetSecond();
		    
		    Location assign_loc(lhs_loc.firstLine, lhs_loc.firstColumn, loc.firstLine, loc.firstColumn);
		    return Outcome<std::unique_ptr<Ast>, Error>(std::make_unique<Assignment>(assign_loc, std::move(composite.GetFirst()), std::move(rhs.GetFirst())));
      }
      // an application expression
      else if (tok == Token::LParen)
      {
        nexttok(); // eat '('

        std::vector<std::unique_ptr<Ast>> args;

        if (tok != Token::RParen)
        {
          // parse the argument list
          do {
            if (tok == Token::Comma)
              nexttok(); // eat ','

            auto arg = ParseAffix(env);

            if (!arg)
              return arg.GetSecond();

            args.emplace_back(std::move(arg.GetFirst()));
            
          } while (tok == Token::Comma);

          // finish parsing the argument list
          if (tok != Token::RParen)
            return Error(Error::Code::MissingRParen, loc);

          nexttok(); // eat ')'
        }
        else // tok == Token::RParen
        {
          nexttok(); // eat ')'
          // the argument list is empty, but this is still an application term.
        }
        
        Location app_loc(lhs_loc.firstLine, lhs_loc.firstColumn, loc.firstLine, loc.firstColumn);
        return Outcome<std::unique_ptr<Ast>, Error>(std::make_unique<Application>(app_loc, std::move(composite.GetFirst()), std::move(args)));
      }
      // else the result is the single composite
      return Outcome<std::unique_ptr<Ast>, Error>(std::move(composite.GetFirst()));
    }


    Outcome<std::unique_ptr<Ast>, Error> Parser::ParseComposite(const Environment& env)
    {
      Location lhs_loc = loc;
      auto dot = ParseDot(env);

      if (!dot)
        return dot.GetSecond();

      if (tok == Token::Op) 
      {
        // this is actually a binop expression
        // where dot is the first left hand side term.
        return ParseInfix(std::move(dot.GetFirst()), 0, env);
      }
      // else the result is simply the dot expression
      return Outcome<std::unique_ptr<Ast>, Error>(std::move(dot.GetFirst()));
    }

    /*
      dot = basic {"." basic}
    */
    Outcome<std::unique_ptr<Ast>, Error> Parser::ParseDot(const Environment& env)
    {
      Location lhs_loc = loc;
      auto left = ParseBasic(env);

      if (!left)
        return left.GetSecond();

      // this is a member access expression
      if (tok == Token::Dot)
      {
        do {
          nexttok(); // eat '.'

          auto right = ParseBasic(env);

          if (!right)
            return right.GetSecond();

          Location dotloc(lhs_loc.firstLine, lhs_loc.firstColumn, loc.firstLine, loc.firstColumn);
          left = Outcome<std::unique_ptr<Ast>, Error>(std::make_unique<Dot>(dotloc, std::move(left.GetFirst()), std::move(right.GetFirst())));
        } while (tok == Token::Dot);
      }
      // either
      return Outcome<std::unique_ptr<Ast>, Error>(std::move(left.GetFirst()));
    }
    
    
    /*
    	This is an implementation of an operator precedence parser
    	going from this pseudo-code:
    	https://en.wikipedia.org/wiki/Operator-precedence_parser
    */
    Outcome<std::unique_ptr<Ast>, Error> Parser::ParseInfix(std::unique_ptr<Ast> lhs, Precedence precedence, const Environment& env)
    {
    	Outcome<std::unique_ptr<Ast>, Error> result(std::move(lhs));
    	
    	InternedString peek_str = nullptr;
    	llvm::Optional<std::pair<InternedString, BinopLiteral*>> peek_opt;
    	BinopLiteral* peek_lit = nullptr;

    	while (TokenToBool(tok) && (tok == Token::Op)  
    		&& (peek_str = env.operators->Intern(txt))
    		&& (peek_opt = env.binops->Lookup(peek_str)) && (peek_opt.has_value()) 
    		&& (peek_lit = peek_opt->second) && (peek_lit->precedence >= precedence))
    	{
    		InternedString op_str = peek_str;
    		BinopLiteral*  op_lit = peek_lit;
    		Location       op_loc = loc;
    		
    		nexttok(); // eat the 'operator'
    		
    		Outcome<std::unique_ptr<Ast>, Error> rhs = ParseDot(env);
    		
    		if (!rhs)
    			return rhs.GetSecond(); 
    			
    		while (TokenToBool(tok) && (tok == Token::Op) 
    			&& (peek_str = env.operators->Intern(txt))
    			&& (peek_opt = env.binops->Lookup(peek_str)) && (peek_opt.has_value())
    			&& (peek_lit = peek_opt->second) 
    			&& ((peek_lit->precedence > op_lit->precedence) 
    				|| ((peek_lit->associativity == Associativity::Right)
    				&& (peek_lit->precedence == op_lit->precedence))))
    		{
    			Outcome<std::unique_ptr<Ast>, Error> temp = ParseInfix(std::move(rhs.GetFirst()), peek_lit->precedence, env);
    			
    			if (!temp)
    				return temp.GetSecond();
    			else 
    				rhs = Outcome<std::unique_ptr<Ast>, Error>(std::move(temp.GetFirst()));
    		}
    		
    		Location binop_loc(op_loc.firstLine, op_loc.firstColumn, loc.firstLine, loc.firstColumn);
    		result = Outcome<std::unique_ptr<Ast>, Error>(std::make_unique<Binop>(binop_loc, op_str, std::move(result.GetFirst()),  std::move(rhs.GetFirst())));
    	}
    	
    	return Outcome<std::unique_ptr<Ast>, Error>(std::move(result.GetFirst()));
    }
    
    /*
      basic = id
            | integer
            | operator basic
            | "true"
            | "false"
            | "(" affix {"," affix} ")"
            | "[" affix {"," affix} "]"

    */
    Outcome<std::unique_ptr<Ast>, Error> Parser::ParseBasic(const Environment& env)
    {
    	switch(tok)
    	{
    	case Token::Id:
    	{
    		Location lhs_loc = loc; // save the beginning location
    		InternedString id = env.symbols->Intern(txt); // Intern the identifier
    		
    		nexttok(); // eat the identifier
        
    		return Outcome<std::unique_ptr<Ast>, Error>(std::make_unique<Variable>(lhs_loc, id));
        break;
    	}
    	
    	case Token::Op: // an operator appearing in the basic position is a unop.
    	{
    		Location lhs_loc = loc; // save the lhs location
    		InternedString op = env.operators->Intern(txt);
    		
    		nexttok(); // eat the op

    		// unops all bind to their immediate right hand side
    		auto rhs = ParseBasic(env);    	
    		
    		if (!rhs)
    			return rhs.GetSecond();
    			
    		Location unop_loc(lhs_loc.firstLine, lhs_loc.firstColumn, loc.firstLine, loc.firstColumn);
    		return Outcome<std::unique_ptr<Ast>, Error>(std::make_unique<Unop>(unop_loc, op, std::move(rhs.GetFirst())));
    	}
    	
    	case Token::LParen: // "("
    	{
        Location lhs_loc = loc;
    		nexttok(); // eat the '('

    		auto result = ParseAffix(env);
    		
    		if (!result)
    			return result.GetSecond(); 

        if (tok == Token::Comma)
        {
          std::vector<std::unique_ptr<Ast>> elements;
          elements.emplace_back(std::move(result.GetFirst()));

          do {
            if (tok == Token::Comma)
              nexttok(); // eat ','

            result = Outcome<std::unique_ptr<Ast>, Error>(ParseAffix(env));

            if (!result)
              return result.GetSecond();
            
            elements.emplace_back(std::move(result.GetFirst()));

          } while (tok == Token::Comma);

          if (tok != Token::RParen)
            return Error(Error::Code::MissingRParen, loc);

          nexttok(); // eat ')'
          Location tupleloc(lhs_loc.firstLine, lhs_loc.firstColumn, loc.firstLine, loc.firstColumn);
          return Outcome<std::unique_ptr<Ast>, Error>(std::make_unique<Tuple>(tupleloc, std::move(elements)));
        } 
        else if (tok != Token::RParen)
    		{
    			return Error(Error::Code::MissingRParen, loc);
    		}
    		else 
    		{
    			nexttok(); // eat the ')'
    			return Outcome<std::unique_ptr<Ast>, Error>(std::move(result.GetFirst()));
    		}
    	}

      case Token::LBracket: // "["
      { 
        Location lhs_loc = loc;
        nexttok(); // eat '['

        std::vector<std::unique_ptr<Ast>> members;

        do
        {
          if (tok == Token::Comma)
            nexttok(); // eat ','

          auto member = ParseAffix(env);

          if (!member)
            return member.GetSecond();

          members.emplace_back(std::move(member.GetFirst()));
        } while (tok == Token::Comma);

        if (tok != Token::RBracket)
          return Error(Error::Code::MissingRBracket, loc);

        nexttok(); // eat ']'

        Location array_loc(lhs_loc.firstLine, lhs_loc.firstColumn, loc.firstLine, loc.firstColumn); 
        return Outcome<std::unique_ptr<Ast>, Error>(std::make_unique<Array>(array_loc, std::move(members)));
      }
    	
    	case Token::Nil:
    	{	
    		Location lhs_loc = loc;
    		nexttok(); // eat 'nil'
    		return Outcome<std::unique_ptr<Ast>, Error>(std::make_unique<Nil>(lhs_loc));

    	}
    	
      
      case Token::Int:
      {
        Location lhs_loc = loc;
        int value = std::stoi(txt);
        nexttok(); // eat '[0-9]+'
        return Outcome<std::unique_ptr<Ast>, Error>(std::make_unique<Int>(lhs_loc, value));
      }
        
      case Token::True:
      {
        Location lhs_loc = loc;
        nexttok(); // Eat "true"
        return Outcome<std::unique_ptr<Ast>, Error> (std::make_unique<Bool>(lhs_loc, true));
      }
      
      case Token::False:
      {
        Location lhs_loc = loc;
        nexttok(); // Eat "false"
        return Outcome<std::unique_ptr<Ast>, Error>(std::make_unique<Bool>(lhs_loc, false));
      }

    	default:
    	{
    		return Error(Error::Code::UnknownBasicToken, loc, "parsed: " + txt);
    	}
    	} // !switch(tok)
    }
    
    


    /*
      type = "Int"
           | "Bool"
           | "(" type {"," type} ")"
           | "[" type "x" int "]"
           | "ptr" type
    */
    Outcome<Type*, Error> Parser::ParseType(const Environment& env)
    {
      switch (tok)
      {
      case Token::NilType:
      {
        nexttok(); // eat 'Nil'
        return Outcome<Type*, Error>(env.types->GetNilType());
        break;
      }
        
    	case Token::IntType:
      {
        nexttok(); // Eat "Int"
        return Outcome<Type*, Error>(env.types->GetIntType());
        break;
      }
        
    	case Token::BoolType:
      {
        nexttok(); // Eat "Bool"
        return Outcome<Type*, Error>(env.types->GetBoolType());
        break;
      }

      case Token::LParen:
      {
        nexttok(); // eat '('
        auto left = ParseType(env);

        if (!left)
          return left;

        if (tok == Token::Comma)
        {
          std::vector<Type*> elem_tys = {left.GetFirst()};
          do {
            nexttok(); // eat ','

            auto elem_ty = ParseType(env);

            if (!elem_ty)
              return elem_ty;
            else
              elem_tys.push_back(elem_ty.GetFirst());

          } while (tok == Token::Comma);

          left = env.types->GetTupleType(elem_tys); 
        }

        if (tok != Token::RParen)
        {
          return Error(Error::Code::MissingRParen, loc);
        }
        nexttok(); // eat ')'

        return left;
      }
      
      case Token::LBracket:
      {
        nexttok(); // eat '['

        auto array_type = ParseType(env);

        if (!array_type)
          return array_type;
        
        if (tok != Token::Id || txt != "x")
        {
          return Outcome<Type*, Error>(Error(Error::Code::MissingArrayX, loc, "parsed: " + txt));            
        }

        nexttok(); // eat 'x'
        
        if (tok != Token::Int)
        {
          return Outcome<Type*, Error>(Error(Error::Code::MissingArrayNum, loc, "parsed: " + txt));
        }

        size_t num = std::stoi(txt);

        nexttok(); // eat '[0-9]+'

        if (tok != Token::RBracket)
        {
          return Outcome<Type*, Error>(Error(Error::Code::MissingRBracket, loc, "parsed: " + txt));
        }

        nexttok(); // eat ']'

        return Outcome<Type*, Error>(env.types->GetArrayType(num, array_type.GetFirst()));
      }

      case Token::Ptr:
      {
        nexttok(); // eat "ptr"

        auto type = ParseType(env);

        if (!type)
          return type;

        return Outcome<Type*, Error>(env.types->GetPointerType(type.GetFirst()));
      }

    	default:
    	{
    		return Error(Error::Code::UnknownTypeToken, loc, "parsed: " + txt);
    	}
      }
    }
        
}
