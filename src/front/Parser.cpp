#include "front/Parser.h"

#include "aux/Environment.h"

// Syntactic Forms
#include "ast/Variable.h"
#include "ast/VarRef.h"
#include "ast/Bind.h"
#include "ast/Assignment.h"
#include "ast/Binop.h"
#include "ast/Unop.h"
#include "ast/Block.h"
#include "ast/Function.h"

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


namespace pink {
    Parser::Parser(std::istream* i)
        : lexer(), tok(Token::End), loc(), txt(), input_stream(i)
    {

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

    bool Parser::EndOfInput()
    {
      bool end = true;

      if (input_stream != nullptr)
      {
        end = input_stream->eof();
      }

      return lexer.EndOfInput() && end; 
          
    }

    void Parser::SetIStream(std::istream* i)
    {
      input_stream = i;
      lexer.Reset();
    }

    void Parser::yyfill()
    {
      std::string t;
      std::getline(*input_stream, t, '\n');
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
     *  the full parsing stack looks like
     *
     *  term := affix ';'
     *
     *  affix := basic (operator basic)* // well, this isn't actually a simple
     *                                   // kleen star, but actually an entrance 
     *                                   // to an operator precedence parser.
     *                                   // however this is optional, just like
     *                                   // the kleen star is.
     *
     *  basic := nil
     *         | true
     *         | false
     *         | integer
     *         | operator basic
     *         | '(' affix ')'
     *         | function
     *         | id
     *         | id '=' affix
     *         | id ':=' affix
     *
     *  function := 'fn' id '(' (arg (',' arg)*)? ')' '{' affix* '}'
     *
     *
     *
     *
     *
     */
    Outcome<std::unique_ptr<Ast>, Error> Parser::Parse(std::shared_ptr<Environment> env)
    {
      while (tok == Token::End 
      &&  lexer.EndOfInput() 
      && (input_stream != nullptr)
      && (!input_stream->eof()))
      {
        yyfill();
        nexttok();  
      }

      // We are currently experiencing an issue with the design of the parser,
      // we want to accept as much input as the source can provide, to ensure 
      // we are actually parsing all of the source, or course. And we would 
      // like to allow arbitrary whitespace before and after terms, without 
      // affecting the resulting parse.
      //
      // however, to know if there is any parsable term after some amount of 
      // whitespace at the end of any given file, we currently attempt to parse
      // all of that whitespace as if it did contain some term, so when it
      // turns out that the whitespace after some term does not contain any
      // more terms, we do so within the parser itself. which since it is
      // expecting to parse something, complains and raises this situation to 
      // an error. however, it is perfectly reasonable to have whitespace after 
      // any given term including the final term within the source. 
      // we would like to catch this particular error above the parser, and 
      // then we can simply ignore it, if we have parsed some terms before
      // this, and warn the user if we parsed nothing, and reached the end of
      // the source.
      //
      // since this is the second situation where we are wanting to catch
      // specific errors outside the parser, i am thinking that it makes sense 
      // to assign each error the compiler knows how to catch a number, and
      // then we can catch any particualar error. it also allows us to modify 
      // the error terms themselves to be significantly smaller, because this 
      // allows us to associate error strings with the number, and not carry
      // the error string within the error term which represents the occurance
      // of the error itself. as a byproduct this will speed up the parser in 
      // situations where we are reporting errors. in both space and time.
      //
      // as an aside, with the current setup, it would require almost no work
      // to parse the source past the point where we encounter the first error 
      // term. because we have already separated the point at which we pull in
      // input text, and the parser itself, meaning even after reporting an
      // error, the parser is still set up to parse the input source just past
      // the error term. however, since the parser stops with the parse as 
      // soon as an error occurs, the next thing it parses may very well be the 
      // second half of the term which produced said error, which given it's 
      // incompleteness is bound to produce another error.
      // it poses the possibility to push the parsing of terms out of sync with
      // the occurance of terms within the source text itself! though, no such 
      // possiblity exists if the error occurs during typechecking of a term.

      // if tok == Token::End even after we parsed everything within the given
      // file up until the EOF, then we return the end of file error, to signal 
      // to the caller that the end of input has been reached.
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
    Outcome<std::unique_ptr<Ast>, Error> Parser::ParseTerm(std::shared_ptr<Environment> env)
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
    


    Outcome<std::unique_ptr<Ast>, Error> Parser::ParseAffix(std::shared_ptr<Environment> env)
    {
      Location left_loc = loc; // save the location of the lhs of this affix expr
		  Outcome<std::unique_ptr<Ast>, Error> left(ParseBasic(env)); // parse the initial term
		
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
    
    
    /*
    	This is an implementation of an operator precedence parser
    	going from this pseudo-code:
    	https://en.wikipedia.org/wiki/Operator-precedence_parser
    */
    Outcome<std::unique_ptr<Ast>, Error> Parser::ParseInfix(std::unique_ptr<Ast> lhs, Precedence precedence, std::shared_ptr<Environment> env)
    {
    	Outcome<std::unique_ptr<Ast>, Error> result(std::move(lhs));
    	Token peek_tok;
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
    		&& (peek_str = env->operators->Intern(txt))
    		&& (peek_opt = env->binops->Lookup(peek_str)) && (peek_opt.hasValue()) 
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
    		
    		Outcome<std::unique_ptr<Ast>, Error> rhs = ParseBasic(env);
    		
    		if (!rhs)
    			return Outcome<std::unique_ptr<Ast>, Error>(rhs.GetTwo()); 
    			
    		while (TokenToBool(tok) && (tok == Token::Op) 
    			&& (peek_str = env->operators->Intern(txt))
    			&& (peek_opt = env->binops->Lookup(peek_str)) && (peek_opt.hasValue())
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
    
    
    Outcome<std::unique_ptr<Ast>, Error> Parser::ParseBasic(std::shared_ptr<Environment> env)
    {
    	switch(tok)
    	{
    	case Token::Id:
    	{
    		Location lhs_loc = loc; // save the beginning location
    		InternedString id = env->symbols->Intern(txt); // Intern the identifier
    		
    		nexttok(); // eat the identifier
    	  
        while (tok == Token::End && !EndOfInput())
        {
          yyfill();
          nexttok();
        }

    		// an identifer can be followed by a bind expression, an assignment expression,
        // or can be a basic variable reference. 
        // i am debating application syntax, on the one hand it is equally easy
        // to modify the parser to accept either common syntax, in BNF snippets
        // 
        // lambda calculus style,
        // 
        // app := affix (affix)+
        // 
        //
        // or c style,
        //
        // app := id '(' (affix (',' affix)*)? ')'
        //
        // it's really a difference of looping when we parse a full affix
        // until we reach the semicolon ending the application; or looping when we 
        // Parse a comma after a full affix until we reach the right parenthesis ending the
        // application. And a difference of an identifier being the only
        // possible token which predicts a possible application term, and any
        // possible affix expression being what possibly predicts an
        // application. 
        // this second point has implications when we consider the fact that
        // within C, functions can never be temporary values, and so
        // identifiers will always be uniquely present when a function is
        // present, and the fact that within the lambda calculus a function is 
        // allowed to be an anonymous value.
        //
        //
        //
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
    		else if (tok == Token::Equals) // an assignment expression
        {
		      nexttok(); // eat '='

		      Outcome<std::unique_ptr<Ast>, Error> rhs(ParseAffix(env));
		        
		      if (!rhs)
		        return Outcome<std::unique_ptr<Ast>, Error>(rhs.GetTwo());
		        
		        // loc holds the location of the rhs of the term after the above call to ParseTerm
		      Location assign_loc(lhs_loc.firstLine, lhs_loc.firstColumn, loc.firstLine, loc.firstColumn);
		      Outcome<std::unique_ptr<Ast>, Error> result(std::make_unique<Assignment>(assign_loc, std::make_unique<VarRef>(lhs_loc, id), std::move(rhs.GetOne())));
		        
    		  return Outcome<std::unique_ptr<Ast>, Error>(std::move(result.GetOne()));
        }
        else // this identifer was simply a variable reference 
        {
          Outcome<std::unique_ptr<Ast>, Error> result(std::make_unique<Variable>(lhs_loc, id));
    			
    			return Outcome<std::unique_ptr<Ast>, Error>(std::move(result.GetOne()));
        }
        break;
    	}
    	
    	case Token::Op: // an operator appearing in the basic position is a unop.
    	{
    		Location lhs_loc = loc; // save the lhs location
    		InternedString op = env->operators->Intern(txt);
    		
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

    		if (tok != Token::RParen)
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
      
      case Token::Fn:
      {
        return ParseFunction(env);
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
    Outcome<std::unique_ptr<Ast>, Error> Parser::ParseBlock(std::shared_ptr<Environment> env)
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

    
    Outcome<std::unique_ptr<Ast>, Error> Parser::ParseFunction(std::shared_ptr<Environment> env)
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
    	
    	name = env->symbols->Intern(txt); // intern the functions name
		
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
                                                                             env->bindings.get())
                                                 );		
    }
    
    
    
    Outcome<std::pair<InternedString, Type*>, Error> Parser::ParseArgument(std::shared_ptr<Environment> env)
    {
    	InternedString name;

    	if (tok != Token::Id)
    	{
    		Error error(Error::Code::MissingArgName, loc);
    		return Outcome<std::pair<InternedString, Type*>, Error>(error);
    	}
    	
    	name = env->symbols->Intern(txt);
    	
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

    	Outcome<Type*, Error> type_res = ParseBasicType(env);
    	
    	if (!type_res)
    	{
    		return Outcome<std::pair<InternedString, Type*>, Error>(type_res.GetTwo());
    	}
    	
    	return Outcome<std::pair<InternedString, Type*>, Error>(std::make_pair(name, type_res.GetOne()));
    }
    
    
    
    Outcome<Type*, Error> Parser::ParseBasicType(std::shared_ptr<Environment> env)
    {
    	switch(tok)
    	{
    	case Token::NilType:
        {
        	Location lhs_loc = loc;
        	nexttok(); // eat 'Nil'
        	Outcome<Type*, Error> result(env->types->GetNilType());
        	return result;
        }
        
    	case Token::IntType:
        {
        	Location lhs_loc = loc;
        	nexttok(); // Eat "Int"
        	Outcome<Type*, Error> result(env->types->GetIntType());
        	return result;
        }
        
    	case Token::BoolType:
        {
        	Location lhs_loc = loc;
        	nexttok(); // Eat "Bool"
        	Outcome<Type*, Error> result(env->types->GetBoolType());
        	return result;
        }
        
    	default:
    	{
    		Error error(Error::Code::UnknownTypeToken, loc);
    		return Outcome<Type*, Error>(error);
    	}
    	}
    }
        
}
