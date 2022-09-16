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
      bool end = input_stream->eof();
      return lexer.EndOfInput() && end;     
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
      // while the lexer is at the end of it's buffer
      // and the input stream is not at the end of it's buffer
      // we can get more input and fill the lexer with it.
      // then we can prime the current token by calling the lexer.
      while (lexer.EndOfInput() 
         && (!input_stream->eof()))
      {
        yyfill();
        nexttok();  
      }

      // if tok == Token::End even after we tried to get more input from the given
      // file it means we read to the EOF, so we return the end of file error, to signal 
      // to the caller that the end of the associated input has been reached.
      if (tok == Token::End)
      {
        return Outcome<std::unique_ptr<Ast>, Error>(Error(Error::Code::EndOfFile, loc)); 
      }

    	return ParseTerm(env);	
    }

    /*
     *  A term is simply any affix expression followed by a semicolon.
     *  
     *  term := affix ';'
     *
     */
    Outcome<std::unique_ptr<Ast>, Error> Parser::ParseTerm(const Environment& env)
    {

      Outcome<std::unique_ptr<Ast>, Error> term(ParseAffix(env));

      while (tok == Token::End && !EndOfInput())
      {
        yyfill();
        nexttok();
      }

      if (tok != Token::Semicolon)
      {
        Error error(Error::Code::MissingSemicolon, loc);
        return Outcome<std::unique_ptr<Ast>, Error>(error);
      }
      // else
      nexttok(); // eat ';'

      if (term)
        return Outcome<std::unique_ptr<Ast>, Error>(std::move(term.GetOne()));
      else 
        return Outcome<std::unique_ptr<Ast>, Error>(term.GetTwo());
    }
    


    Outcome<std::unique_ptr<Ast>, Error> Parser::ParseAffix(const Environment& env)
    {
      if (tok == Token::Fn)
      {
        return ParseFunction(env);
      }
      else
      {
        Outcome<std::unique_ptr<Ast>, Error> left(ParseComposite(env)); // parse the initial term
        if (!left) // if the previous parse failed, return the error immediately
          return Outcome<std::unique_ptr<Ast>, Error>(left.GetTwo());

        // we just parsed and are about to look at more input to see what to do
        // next. this means we have to account for the case where that more input 
        // appears on the next textual line.
        while (tok == Token::End && !EndOfInput())
        {
          yyfill();
          nexttok();
        }
        
        if (tok == Token::Op) // we assume this is a binary operator appearing after a basic term
        {
          // Handle the entire binop expression with the Infix Parser
          return ParseInfix(std::move(left.GetOne()), 0, env); // pass or fail, return the result of parsing a binop expression.
        }
        else // the single term we parsed at the beginning is the result of this parse.
        {
          return Outcome<std::unique_ptr<Ast>, Error>(std::move(left.GetOne()));
        }
      }
    }


    Outcome<std::unique_ptr<Ast>, Error> Parser::ParseComposite(const Environment& env)
    {
      Location lhs_loc = loc;
      Outcome<std::unique_ptr<Ast>, Error> left(ParseDot(env));
      
      if (!left) // if the previous parse failed, return the error immediately
			  return Outcome<std::unique_ptr<Ast>, Error>(left.GetTwo());

      // we just parsed some input and are about to look at more to see what to do
      // next. this means we have to account for the case where that more input 
      // appears on the next textual line.
      while (tok == Token::End && !EndOfInput())
      {
        yyfill();
        nexttok();
      }
      

      // dot = affix
    	if (tok == Token::Equals) // an assignment expression
      {
		    nexttok(); // eat '='
        
        while (tok == Token::End && !EndOfInput())
        {
          yyfill();
          nexttok();
        }
		    
        Outcome<std::unique_ptr<Ast>, Error> rhs(ParseAffix(env));
		        
		    if (!rhs)
		      return Outcome<std::unique_ptr<Ast>, Error>(rhs.GetTwo());
		       
		    // loc holds the location of the rhs of the term after the above call to ParseTerm
		    Location assign_loc(lhs_loc.firstLine, lhs_loc.firstColumn, loc.firstLine, loc.firstColumn);
		    Outcome<std::unique_ptr<Ast>, Error> result(std::make_unique<Assignment>(assign_loc, std::move(left.GetOne()), std::move(rhs.GetOne())));
		        
    	  return Outcome<std::unique_ptr<Ast>, Error>(std::move(result.GetOne()));
      }      
      // dot '(' (affix (',' affix)*)? ')'
      else if (tok == Token::LParen) // an application expression
      {
        nexttok(); // eat '('

        while (tok == Token::End && !EndOfInput())
        {
          yyfill();
          nexttok();
        }

        std::vector<std::unique_ptr<Ast>> args;

        if (tok != Token::RParen)
        {
          // parse the argument list
          do {
            if (tok == Token::Comma)
            {
              nexttok(); // eat ','
            }

            while (tok == Token::End && !EndOfInput())
            {
              yyfill();
              nexttok();
            }

            Outcome<std::unique_ptr<Ast>, Error> arg(ParseAffix(env));

            if (!arg)
              return Outcome<std::unique_ptr<Ast>, Error>(arg.GetTwo());

            args.emplace_back(std::move(arg.GetOne()));
            
          } while (tok == Token::Comma);
          // finish parsing the argument list
          if (tok != Token::RParen)
          {
            Error error(Error::Code::MissingRParen, loc);
            return Outcome<std::unique_ptr<Ast>, Error>(error);
          }
          nexttok(); // eat ')'
        }
        else // tok == Token::RParen
        {
          nexttok(); // eat ')'
          // the argument list is empty, but this is still an application term.
        }
        
        Location app_loc(lhs_loc.firstLine, lhs_loc.firstColumn, loc.firstLine, loc.firstColumn);
        return Outcome<std::unique_ptr<Ast>, Error>(std::make_unique<Application>(app_loc, std::move(left.GetOne()), std::move(args)));
      }
      else // the single term we parsed is the result.
      {
        return Outcome<std::unique_ptr<Ast>, Error>(std::move(left.GetOne()));
      }
    }

    Outcome<std::unique_ptr<Ast>, Error> Parser::ParseDot(const Environment& env)
    {
      Location lhs_loc = loc;
      Outcome<std::unique_ptr<Ast>, Error> left(ParseBasic(env));

      if (!left)
        return Outcome<std::unique_ptr<Ast>, Error>(left.GetTwo());

      while (tok == Token::End && !EndOfInput())
      {
        yyfill();
        nexttok();
      }

      // basic '.' affix
      if (tok == Token::Dot)
      {
        do {
          nexttok(); // eat '.'

          while (tok == Token::End && !EndOfInput())
          {
            yyfill();
            nexttok();
          }

          Outcome<std::unique_ptr<Ast>, Error> right(ParseBasic(env));

          if (!right)
            return Outcome<std::unique_ptr<Ast>, Error>(right.GetTwo());

          Location dotloc(lhs_loc.firstLine, lhs_loc.firstColumn, loc.firstLine, loc.firstColumn);
          left = Outcome<std::unique_ptr<Ast>, Error>(std::make_unique<Dot>(dotloc, std::move(left.GetOne()), std::move(right.GetOne())));
        } while (tok == Token::Dot);
        
        return Outcome<std::unique_ptr<Ast>, Error>(std::move(left.GetOne()));
      }
      else
      {
        return Outcome<std::unique_ptr<Ast>, Error>(std::move(left.GetOne()));
      }
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
     
      // if the operator we are trying to parse is not yet known to the
      // compiler, we don't want to bail on the parse, because that can leave 
      // the parser having only halfway consumed the term of the langauge. 
      // meaning the next few calls to parse may or may not correspond to
      // single terms within the language. this is a problem not because
      // the error occurs necessarily, but because of the model of the compiler 
      // itself. For an implementation of use-before-definition we are going to 
      // need some way to keep track of the text of each given full term of the
      // language which we parse from the source code. Then we will have access
      // to say, a given usage of an unknown name, or in this case an unknown
      // binop. Then later when the definition is parsed and known, we can
      // reparse the binary expression against the correct precedence and
      // associativity. 
      //
      // Or later when we encounter the definition of a given function being 
      // applied within an earlier defined function we can then go back and 
      // properly type the earlier function. (though, since this happens at the
      // level of types, we don't need to reparse the definition of the
      // procedure to type it once we have a definition for a given name.
      //
      // (if we think about it, the reason this situation is distinct from the
      // binops situation is that the parser can already construct valid
      // Variable terms from unknown names, because the parser does not need to 
      // know the definition of the name to parse it as a name. whereas with
      // the binop situation, part of the definition of the binop itself is
      // being used by the parser so that it can properly parse the given
      // source text.)
      // in code this translates roughly to,
      //  -) before we attempt to use any given operators precedence and
      //  associativity, we check that it exists, and if it doesn't we 
      //  construct a default precedence and associativity for it.
      // to accomplish this we modified the definition of
      // env->binops->Lookup(), to return a default implementation instead 
      // of no definition, this effectively means that 
      // (peek_opt = env->binops->Lookup(peek_str)) is always "true"
      // in that it always exists. so now, we won't catch the fact that we
      // parsed an undefined binop until we attempt to type the binop, 
      // and realize that there is no implementation of the binop for the types 
      // the binop expression is passing. Thus, the parser will not fail to
      // parse the term in this situation, and the error is still handled before 
      // compilation is finished. although, it is a materally different error
      // than before. and to retreive the previous error, we would need to
      // check if the binop definition is default or not at the point when we
      // fail to find an implementation for the given types.

    	while (TokenToBool(tok) && (tok == Token::Op)  
    		&& (peek_str = env.operators->Intern(txt))
    		&& (peek_opt = env.binops->Lookup(peek_str)) && (peek_opt.hasValue()) 
    		&& (peek_lit = peek_opt->second) && (peek_lit->precedence >= precedence))
    	{
    		InternedString op_str = peek_str;
    		BinopLiteral*  op_lit = peek_lit;
    		Location       op_loc = loc;
    		
    		nexttok(); // eat the operator, Token::Op

        // if we are trying to parse a binop expression,
        // and we ran into the end of the lexers buffer 
        // before we run into the rhs of the term, we 
        // are probably in the situation where the rhs 
        // appears on the next line of input, thus 
        // we use yyfill to retreive the next line, and
        // then nexttok() to lex the next line.
        while (tok == Token::End && !EndOfInput())
        {
          yyfill();
          nexttok();
        } 
    		
    		Outcome<std::unique_ptr<Ast>, Error> rhs = ParseComposite(env);
    		
    		if (!rhs)
    			return Outcome<std::unique_ptr<Ast>, Error>(rhs.GetTwo()); 
    			
    		while (TokenToBool(tok) && (tok == Token::Op) 
    			&& (peek_str = env.operators->Intern(txt))
    			&& (peek_opt = env.binops->Lookup(peek_str)) && (peek_opt.hasValue())
    			&& (peek_lit = peek_opt->second) 
    			&& ((peek_lit->precedence > op_lit->precedence) 
    				|| ((peek_lit->associativity == Associativity::Right)
    				&& (peek_lit->precedence == op_lit->precedence))))
    		{
    			Outcome<std::unique_ptr<Ast>, Error> temp(ParseInfix(std::move(rhs.GetOne()), peek_lit->precedence, env));
    			
    			if (!temp)
    				return Outcome<std::unique_ptr<Ast>, Error>(temp.GetTwo());
    			else 
    			{
    				rhs = Outcome<std::unique_ptr<Ast>, Error>(std::move(temp.GetOne()));
    			}
    		}
    		
    		// in the case where the binop is not known already, 
    		// we cannot continue to parse the potential binop expression.
    		// since this case break out of the while loop above, this is 
    		// the place in the code to check for it occuring
    		if (!peek_opt.hasValue())
    		{
				Error error(Error::Code::UnknownBinop, loc);
				return Outcome<std::unique_ptr<Ast>, Error>(error);
    		}
    		
    		Location binop_loc(op_loc.firstLine, op_loc.firstColumn, loc.firstLine, loc.firstColumn);
    		result = Outcome<std::unique_ptr<Ast>, Error>(std::make_unique<Binop>(binop_loc, op_str, std::move(result.GetOne()),  std::move(rhs.GetOne())));
    	}
    	
    	// in the case where the binop is not known already, 
		// we cannot continue to parse the potential binop expression.
		// since this case break out of the while loop above, this is 
		// the place in the code to check for it occuring
    	if (!peek_opt.hasValue())
    	{
    		Error error(Error::Code::UnknownBinop, loc);
    		return Outcome<std::unique_ptr<Ast>, Error>(error);
    	}
    	
    	return Outcome<std::unique_ptr<Ast>, Error>(std::move(result.GetOne()));
    }
    
    
    Outcome<std::unique_ptr<Ast>, Error> Parser::ParseBasic(const Environment& env)
    {
    	switch(tok)
    	{
    	case Token::Id:
    	{
    		Location lhs_loc = loc; // save the beginning location
    		InternedString id = env.symbols->Intern(txt); // Intern the identifier
    		
    		nexttok(); // eat the identifier
        // id := affix 
        if (tok == Token::ColonEq) // a bind expression
    	  {
    		  nexttok(); // eat the ':='
    			
    		  Outcome<std::unique_ptr<Ast>, Error> rhs(ParseAffix(env)); // parse the right hand side of the binding
    		
    		  if (!rhs)
    			  return Outcome<std::unique_ptr<Ast>, Error>(rhs.GetTwo());
    				
    		  Location bind_loc(lhs_loc.firstLine, lhs_loc.firstColumn, loc.firstLine, loc.firstColumn);
    		  Outcome<std::unique_ptr<Ast>, Error> result(std::make_unique<Bind>(bind_loc, id, std::move(rhs.GetOne())));
    			
    		  return Outcome<std::unique_ptr<Ast>, Error>(std::move(result.GetOne()));
    	  }
        else
        {
    		  Outcome<std::unique_ptr<Ast>, Error> result(std::make_unique<Variable>(lhs_loc, id));
    			
    	  	return Outcome<std::unique_ptr<Ast>, Error>(std::move(result.GetOne()));
        }
        break;
    	}
    	
    	case Token::Op: // an operator appearing in the basic position is a unop.
    	{
    		Location lhs_loc = loc; // save the lhs location
    		InternedString op = env.operators->Intern(txt);
    		
    		nexttok(); // eat the op
    	
        while (tok == Token::End && !EndOfInput())
        {
          yyfill();
          nexttok();
        }

    		// unops all bind to their immediate right hand side
    		Outcome<std::unique_ptr<Ast>, Error> rhs(ParseBasic(env));    	
    		
    		if (!rhs)
    			return Outcome<std::unique_ptr<Ast>, Error>(rhs.GetTwo());
    			
    		Location unop_loc(lhs_loc.firstLine, lhs_loc.firstColumn, loc.firstLine, loc.firstColumn);
    		Outcome<std::unique_ptr<Ast>, Error> result(std::make_unique<Unop>(unop_loc, op, std::move(rhs.GetOne())));

    		return Outcome<std::unique_ptr<Ast>, Error>(std::move(result.GetOne()));
    	}
    	
    	case Token::LParen:
    	{
        Location lhs_loc = loc;
    		nexttok(); // eat the '('
    		
        while (tok == Token::End && !EndOfInput())
        {
          yyfill();
          nexttok();
        }

        // we recur up to affix, because while we want to allow complex
        // expressions to occur within parenthesis. especially to allow 
        // for composing mathematical expressions within the 
        // syntax of the language. parenthesis are the natural way of 
        // reconfiguring the precedence of evaluation of the written 
        // expression. 
        //
        // it would be valid to recur up to ParseTerm, except that it would 
        // introduce the extra syntax of a postfix ';' to the parenthized expression.
        // which is simply strange syntax.
    		Outcome<std::unique_ptr<Ast>, Error> result(ParseAffix(env));
    		
    		if (!result)
    			return Outcome<std::unique_ptr<Ast>, Error>(result.GetTwo());
    	
        while (tok == Token::End && !EndOfInput())
        {
          yyfill();
          nexttok();
        } 

        if (tok == Token::Comma)
        {
          std::vector<std::unique_ptr<Ast>> elements;
          elements.emplace_back(std::move(result.GetOne()));

          do {
            if (tok == Token::Comma)
              nexttok(); // eat ','

            while (tok == Token::End && !EndOfInput())
            {
              yyfill();
              nexttok();
            }

            result = ParseAffix(env);

            if (!result)
              return Outcome<std::unique_ptr<Ast>, Error>(result.GetTwo());
            
            elements.emplace_back(std::move(result.GetOne()));

          } while (tok == Token::Comma);

          if (tok != Token::RParen)
          {
            Error error(Error::Code::MissingRParen, loc);
            return Outcome<std::unique_ptr<Ast>, Error>(error);
          }

          nexttok(); // eat ')'
          Location tupleloc(lhs_loc.firstLine, lhs_loc.firstColumn, loc.firstLine, loc.firstColumn);
          return Outcome<std::unique_ptr<Ast>, Error>(std::make_unique<Tuple>(tupleloc, std::move(elements)));
        } 
        else if (tok != Token::RParen)
    		{
    			Error error(Error::Code::MissingRParen, loc);
    			return Outcome<std::unique_ptr<Ast>, Error>(error);
    		}
    		else 
    		{
    			nexttok(); // eat the ')'
    			return Outcome<std::unique_ptr<Ast>, Error>(std::move(result.GetOne()));
    		}
    	}

      case Token::LBracket:
      { 
        Location lhs_loc = loc;
        nexttok(); // eat '['
        
        while (tok == Token::End && !EndOfInput())
        {
          yyfill();
          nexttok();
        }

        std::vector<std::unique_ptr<Ast>> members;

        do
        {
          if (tok == Token::Comma)
          {
            nexttok(); // eat ','

            while (tok == Token::End && !EndOfInput())
            {
              yyfill();
              nexttok();
            }
          }

          Outcome<std::unique_ptr<Ast>, Error> member = ParseAffix(env);

          if (!member)
            return Outcome<std::unique_ptr<Ast>, Error>(member.GetTwo());

          members.emplace_back(std::move(member.GetOne()));

          while (tok == Token::End && !EndOfInput())
          {
            yyfill();
            nexttok();
          }

        } while (tok == Token::Comma);

        while (tok == Token::End && EndOfInput())
        {
          yyfill();
          nexttok();
        }

        if (tok != Token::RBracket)
        {
          return Outcome<std::unique_ptr<Ast>, Error>(Error(Error::Code::MissingRBracket, loc));
        }

        nexttok(); // eat ']'
        Location array_loc(lhs_loc.firstLine, lhs_loc.firstColumn, loc.firstLine, loc.firstColumn); 
        return Outcome<std::unique_ptr<Ast>, Error>(std::make_unique<Array>(array_loc, std::move(members)));
      }
    	
    	case Token::Nil:
    	{	
    		Location lhs_loc = loc;
    		nexttok(); // eat 'nil'
    		Outcome<std::unique_ptr<Ast>, Error> result(std::make_unique<Nil>(lhs_loc));
    		
    		return Outcome<std::unique_ptr<Ast>, Error>(std::move(result.GetOne()));

    	}
    	
      
      case Token::Int:
      {
        Location lhs_loc = loc;
        int value = std::stoi(txt);
        nexttok(); // eat '[0-9]+'
      
        Outcome<std::unique_ptr<Ast>, Error> result(std::make_unique<Int>(lhs_loc, value));

        return Outcome<std::unique_ptr<Ast>, Error>(std::move(result.GetOne()));
      }
        
      case Token::True:
      {
        Location lhs_loc = loc;
        nexttok(); // Eat "true"
        Outcome<std::unique_ptr<Ast>, Error> result(std::make_unique<Bool>(lhs_loc, true));
        
        return Outcome<std::unique_ptr<Ast>, Error>(std::move(result.GetOne()));
      }
      
      case Token::False:
      {
        Location lhs_loc = loc;
        nexttok(); // Eat "false"
        Outcome<std::unique_ptr<Ast>, Error> result(std::make_unique<Bool>(lhs_loc, false));

        return Outcome<std::unique_ptr<Ast>, Error>(std::move(result.GetOne()));
      }

      case Token::If:
      {
        return ParseConditional(env);
      }

      case Token::While:
      {
        return ParseWhile(env);
      }


        
    	default:
    	{
    		Error error(Error::Code::UnknownBasicToken, loc);
    		return Outcome<std::unique_ptr<Ast>, Error>(error);
    	}
    	} // !switch(tok)
    }
    
    
    /*  block = '{' affix (affix)* '}'
     *
     *  
     *
     *
     *
     *
     *
     */
    Outcome<std::unique_ptr<Ast>, Error> Parser::ParseBlock(const Environment& env)
    {
    	std::vector<std::unique_ptr<Ast>> stmnts;
    	Outcome<std::unique_ptr<Ast>, Error> result(Error(Error::Code::None, loc));
    	Location left_loc = loc;

      if (tok != Token::LBrace)
		  {
			  Error error(Error::Code::MissingLBrace, loc);
			  return Outcome<std::unique_ptr<Ast>, Error>(error);
		  }
		
		  nexttok(); // eat '{'

      while (tok == Token::End && !EndOfInput())
      {
        yyfill();
        nexttok();
      }
    	
    	do {		
    		// parse a term.
        // we recurr up to ParseTerm which requires the expression we parse to 
        // end in a semicolon, that way, a block is composed of a sequence of 
        // semicolon separated affix expressions.
    		result = ParseTerm(env);
    		
    		if (!result)
    			return Outcome<std::unique_ptr<Ast>, Error>(result.GetTwo());
    		
    		// add it to the current block
    		stmnts.emplace_back(std::move(result.GetOne()));
       
        // we just parsed some portion of this block, and reached the
        // end of the lexers buffer before we are done, if the input_stream
        // is not yet empty, all this means is that the rest of this block 
        // continues on the next line of source, which we have yet to pick 
        // up.
        while (tok == Token::End && !EndOfInput())
        {
          yyfill();
          nexttok();
        }
    	
      } while (tok != Token::RBrace);

      if (tok != Token::RBrace)
      {
        Error error(Error::Code::MissingRBrace, loc);
        return Outcome<std::unique_ptr<Ast>, Error>(error);
      }

      nexttok(); // eat '}'
    	
    	Location block_loc(left_loc.firstLine, left_loc.firstColumn, loc.firstLine, loc.firstColumn);
    	return Outcome<std::unique_ptr<Ast>, Error>(std::make_unique<Block>(block_loc, stmnts));
    }


    Outcome<std::unique_ptr<Ast>, Error> Parser::ParseConditional(const Environment& env)
    {
      Location lhs_loc = loc;
      
      if (tok != Token::If)
      {
        return Outcome<std::unique_ptr<Ast>, Error>(Error(Error::Code::MissingIf, loc));
      }

      nexttok(); // eat 'if'

      while (tok == Token::End && !EndOfInput())
      {
        yyfill();
        nexttok();
      }
      
      Outcome<std::unique_ptr<Ast>, Error> test_term = ParseAffix(env);

      if (!test_term)
        return Outcome<std::unique_ptr<Ast>, Error>(test_term.GetTwo());

      while (tok == Token::End && !EndOfInput())
      {
        yyfill();
        nexttok();
      }
      
      if (tok != Token::Then)
      {
        return Outcome<std::unique_ptr<Ast>, Error>(Error(Error::Code::MissingThen, loc));
      }

      nexttok(); // eat 'then'

      while (tok == Token::End && !EndOfInput())
      {
        yyfill();
        nexttok();
      }
      
      Outcome<std::unique_ptr<Ast>, Error> then_term = ParseBlock(env);

      if (!then_term)
        return Outcome<std::unique_ptr<Ast>, Error>(then_term.GetTwo());

      while (tok == Token::End && !EndOfInput())
      {
        yyfill();
        nexttok();
      }
      if (tok != Token::Else)
      {
        return Outcome<std::unique_ptr<Ast>, Error>(Error(Error::Code::MissingElse, loc));
      }

      nexttok(); // eat 'else'
      
      while (tok == Token::End && !EndOfInput())
      {
        yyfill();
        nexttok();
      }

      Outcome<std::unique_ptr<Ast>, Error> else_term = ParseBlock(env);

      if (!else_term)
        return Outcome<std::unique_ptr<Ast>, Error>(else_term.GetTwo());

      Location condloc(lhs_loc.firstLine, lhs_loc.firstColumn, loc.firstLine, loc.firstColumn);
      return Outcome<std::unique_ptr<Ast>, Error>(std::make_unique<Conditional>(condloc, std::move(test_term.GetOne()), std::move(then_term.GetOne()), std::move(else_term.GetOne())));
    }

    Outcome<std::unique_ptr<Ast>, Error> Parser::ParseWhile(const Environment& env)
    {
      Location lhs_loc = loc;
      if (tok != Token::While)
      {
        return Outcome<std::unique_ptr<Ast>, Error>(Error(Error::Code::MissingWhile, loc));
      }
      
      nexttok(); // eat 'while'

      while (tok == Token::End && !EndOfInput())
      {
        yyfill();
        nexttok();
      }

      Outcome<std::unique_ptr<Ast>, Error> test_term = ParseAffix(env);

      if (!test_term)
        return Outcome<std::unique_ptr<Ast>, Error>(test_term.GetTwo());

      while (tok == Token::End && !EndOfInput())
      {
        yyfill();
        nexttok();
      }

      if (tok != Token::Do)
      {
        return Outcome<std::unique_ptr<Ast>, Error>(Error(Error::Code::MissingDo, loc));
      }

      nexttok(); // eat 'do'

      while (tok == Token::End && !EndOfInput())
      {
        yyfill();
        nexttok();
      }

      Outcome<std::unique_ptr<Ast>, Error> body_term = ParseBlock(env);

      if (!body_term)
        return Outcome<std::unique_ptr<Ast>, Error>(body_term.GetTwo());

      Location whileloc(lhs_loc.firstLine, lhs_loc.firstColumn, loc.firstLine, loc.firstColumn);
      return Outcome<std::unique_ptr<Ast>, Error>(std::make_unique<While>(whileloc, std::move(test_term.GetOne()), std::move(body_term.GetOne())));
    }
    
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
    	{
    		Error error(Error::Code::MissingFn, loc);
    		return Outcome<std::unique_ptr<Ast>, Error>(error);
    	}
    	
    	nexttok(); // eat 'fn'
    
      while (tok == Token::End && !EndOfInput())
      {
        yyfill();
        nexttok();
      }  
    	
    	if (tok != Token::Id)
    	{
    		Error error(Error::Code::MissingFnName, loc);
    		return Outcome<std::unique_ptr<Ast>, Error>(error);
    	}
    	
    	name = env.symbols->Intern(txt); // intern the functions name
		
		  nexttok(); // eat 'Id'
	
      while (tok == Token::End && !EndOfInput())
      {
        yyfill();
        nexttok();
      }  

		  if (tok != Token::LParen)
		  {
        Error error(Error::Code::MissingLParen, loc);
        return Outcome<std::unique_ptr<Ast>, Error>(error);
      }
      
      nexttok(); // eat '('
      
      while (tok == Token::End && !EndOfInput())
      {
        yyfill();
        nexttok();
      }

      if (tok == Token::Id) // beginning of the argument list
      {
        do {
          if (tok == Token::Comma)
          {
            nexttok(); // eat ','
          }

          while (tok == Token::End && !EndOfInput())
          {
            yyfill();
            nexttok();
          }
        
          Outcome<std::pair<InternedString, Type*>, Error> arg_res = ParseArgument(env);
          
          if (!arg_res)
          {
            return Outcome<std::unique_ptr<Ast>, Error>(arg_res.GetTwo());
          }
          
          args.emplace_back(arg_res.GetOne());
        } while (tok == Token::Comma);
      }
      
      // handle the case where we just parsed an argument list, and 
      // the case where we parsed a no argument argument list, either 
      // way the next token must be ')'
      if (tok != Token::RParen)
      {
        Error error(Error::Code::MissingRParen, loc);
        return Outcome<std::unique_ptr<Ast>, Error>(error);
      }
      
      nexttok(); // eat ')'
      
      while (tok == Token::End && !EndOfInput())
      {
        yyfill();
        nexttok();
      } 
      
      // parse the body of the function.
      Outcome<std::unique_ptr<Ast>, Error> body_res = ParseBlock(env);
      
      if (!body_res)
      {
        return Outcome<std::unique_ptr<Ast>, Error>(body_res.GetTwo());
      }
      
      Location fn_loc = {lhs_loc.firstLine, lhs_loc.firstColumn, loc.firstLine, loc.firstColumn};
      
      // we have all the parts, build the function.
      return Outcome<std::unique_ptr<Ast>, Error>(std::make_unique<Function>(fn_loc,
                                                                             name, 
                                                                             args, 
                                                                             std::move(body_res.GetOne()), 
                                                                             env.bindings.get())
                                                 );		
    }
    
    
    
    Outcome<std::pair<InternedString, Type*>, Error> Parser::ParseArgument(const Environment& env)
    {
    	InternedString name;

    	if (tok != Token::Id)
    	{
    		Error error(Error::Code::MissingArgName, loc);
    		return Outcome<std::pair<InternedString, Type*>, Error>(error);
    	}
    	
    	name = env.symbols->Intern(txt);
    	
    	nexttok(); // eat 'Id'
    
      while (tok == Token::End && !EndOfInput())
      { 
        yyfill();
        nexttok();
      }

    	if (tok != Token::Colon)
    	{
    		Error error(Error::Code::MissingArgColon, loc);
    		return Outcome<std::pair<InternedString, Type*>, Error>(error);
    	}
    	
    	nexttok(); // eat ':'
    	
      while (tok == Token::End && !EndOfInput())
      {
        yyfill();
        nexttok();
      }

      if (tok == Token::Comma
       || tok == Token::RParen)
      {
        Error error(Error::Code::MissingArgType, loc);
        return Outcome<std::pair<InternedString, Type*>, Error>(error);
      }

    	Outcome<Type*, Error> type_res = ParseType(env);
    	
    	if (!type_res)
    	{
    		return Outcome<std::pair<InternedString, Type*>, Error>(type_res.GetTwo());
    	}
    	
    	return Outcome<std::pair<InternedString, Type*>, Error>(std::make_pair(name, type_res.GetOne()));
    }

    Outcome<Type*, Error> Parser::ParseType(const Environment& env)
    {
      Outcome<Type*, Error> result(Error(Error::Code::None, Location()));
      Outcome<Type*, Error> basic_type(ParseBasicType(env));

      if (!basic_type)
        return basic_type;

      if (tok == Token::Op && txt == "*")
      {
        nexttok(); // eat '*'  
        result = env.types->GetPointerType(basic_type.GetOne());
      }
      else
      {
        result = basic_type;
      }

      return result;
    }    
    
    Outcome<Type*, Error> Parser::ParseBasicType(const Environment& env)
    {
    	switch(tok)
    	{
    	case Token::NilType:
      {
        nexttok(); // eat 'Nil'
        Outcome<Type*, Error> result(env.types->GetNilType());
        return result;
      }
        
    	case Token::IntType:
      {
        nexttok(); // Eat "Int"
        Outcome<Type*, Error> result(env.types->GetIntType());
        return result;
      }
        
    	case Token::BoolType:
      {
        nexttok(); // Eat "Bool"
        Outcome<Type*, Error> result(env.types->GetBoolType());
        return result;
      }

      case Token::LParen:
      {
        nexttok(); // eat '('
        Outcome<Type*, Error> left(ParseType(env));

        if (!left)
          return left;

        if (tok == Token::Comma)
        {
          std::vector<Type*> elem_tys = {left.GetOne()};
          do {
            nexttok(); // eat ','

            Outcome<Type*, Error> elem_ty(ParseType(env));

            if (!elem_ty)
              return elem_ty;
            else
              elem_tys.push_back(elem_ty.GetOne());

          } while (tok == Token::Comma);

          
          left = env.types->GetTupleType(elem_tys); 
        }

        if (tok != Token::RParen)
        {
          Error error(Error::Code::MissingRParen, loc);
          return Outcome<Type*, Error>(error);
        }
        nexttok(); // eat ')'

        return Outcome<Type*, Error>(left);
      }
      
      case Token::LBracket:
      {
        nexttok(); // eat '['

        while (tok == Token::End && !EndOfInput())
        {
          yyfill();
          nexttok();
        }

        Outcome<Type*, Error> array_type = ParseType(env);

        if (!array_type)
          return array_type;
        
        while (tok == Token::End && !EndOfInput())
        {
          yyfill();
          nexttok();
        }
        
        if (tok != Token::Id || txt != "x")
        {
          return Outcome<Type*, Error>(Error(Error::Code::MissingArrayX, loc));            
        }

        nexttok(); // eat 'x'
        
        while (tok == Token::End && !EndOfInput())
        {
          yyfill();
          nexttok();
        }
        
        if (tok != Token::Int)
        {
          return Outcome<Type*, Error>(Error(Error::Code::MissingArrayNum, loc));
        }

        size_t num = std::stoi(txt);

        nexttok(); // eat '[0-9]+'

        while (tok == Token::End && !EndOfInput())
        {
          yyfill();
          nexttok();
        }

        if (tok != Token::RBracket)
        {
          return Outcome<Type*, Error>(Error(Error::Code::MissingRBracket, loc));
        }

        nexttok(); // eat ']'

        return Outcome<Type*, Error>(env.types->GetArrayType(num, array_type.GetOne()));
      }

    	default:
    	{
    		Error error(Error::Code::UnknownTypeToken, loc);
    		return Outcome<Type*, Error>(error);
    	}
    	}
    }
        
}
