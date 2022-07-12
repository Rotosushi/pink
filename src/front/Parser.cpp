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
    Parser::Parser()
        : lexer(), tok(Token::Error), loc(), txt(), input_stream(nullptr)
    {

    }

    Parser::~Parser()
    {

    }

    std::string& Parser::GetBuf()
    {
      return lexer.GetBuf();
    }

    void Parser::yyfill()
    {
      if (lexer.EndOfInput())
      {
        std::string t;
      
        if (input_stream != nullptr)
        {
          std::getline(*input_stream, t, '\n');
          lexer.AppendBuf(t);
        }
      } 
    }

    void Parser::nexttok()
    {
        tok = lexer.yylex();
        loc = lexer.yyloc();
        txt = lexer.yytxt();
    }

    Outcome<std::unique_ptr<Ast>, Error> Parser::Parse(std::string str, std::shared_ptr<Environment> env, std::istream* input_stream)
    {
      this->input_stream = input_stream;
      return Parse(str, env);
    }

    Outcome<std::unique_ptr<Ast>, Error> Parser::Parse(std::string str, std::shared_ptr<Environment> env)
    {
    	tok = Token::Error;
    	loc = Location();
    	txt = "";
    	
    	if (str != txt)
    	{
		    lexer.SetBuf(str);
		    nexttok(); // prime the lexers
		    return ParseAffix(env);
        }
        else
        {
        	// nothing is in the input text, so we signal that by constructing a nil literal.
        	// we could also construct a special Ast node 'Empty' which has the type 'Nil'
        	// but that would not be materially different than a nil literal.
        	// a nil literal appearing at the end of a file then on it's own would generate no code.
        	// which means the empty text at the end of an input file would also generate no code. 		
    		return Outcome<std::unique_ptr<Ast>, Error>(std::make_unique<Nil>(loc));
        }
    }

    
    Outcome<std::unique_ptr<Ast>, Error> Parser::ParseAffix(std::shared_ptr<Environment> env)
    {
      Location left_loc = loc; // save the location of the lhs of this affix expr
		  Outcome<std::unique_ptr<Ast>, Error> left(ParseBasic(env)); // parse the initial term
		
		  if (!left) // if the previous parse failed, return the error immediately
			  return Outcome<std::unique_ptr<Ast>, Error>(left.GetTwo());
		
        if (tok == Token::Op) // we assume this is a binary operator appearing after a basic term
        {
        	// Handle the entire binop expression with the Infix Parser
            return ParseInfix(std::move(left.GetOne()), 0, env); // pass or fail, return the result of parsing a binop expression.
        }
        else // the single term we parsed at the beginning is the result of this parse.
        	return Outcome<std::unique_ptr<Ast>, Error>(std::move(left.GetOne()));
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
    	
    	while (TokenToBool(peek_tok = tok) && (peek_tok == Token::Op)  
    		&& (peek_str = env->operators->Intern(txt))
    		&& (peek_opt = env->binops->Lookup(peek_str)) && (peek_opt.hasValue()) 
    		&& (peek_lit = peek_opt->second) && (peek_lit->precedence >= precedence))
    	{
    		InternedString op_str = peek_str;
    		BinopLiteral*  op_lit = peek_lit;
    		Location       op_loc = loc;
    		
    		nexttok(); // eat the operator, Token::Op 
    		
    		Outcome<std::unique_ptr<Ast>, Error> rhs = ParseBasic(env);
    		
    		if (!rhs)
    			return Outcome<std::unique_ptr<Ast>, Error>(rhs.GetTwo()); 
    			
    		while (TokenToBool(peek_tok = tok) && (peek_tok == Token::Op) 
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
				Error error(Error::Kind::Syntax,
					std::string("[") + peek_str + std::string("] binop is unknown, and thus has no precedence to parse against"),
					loc);
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
    		Error error(Error::Kind::Syntax,
    			std::string("[") + peek_str + std::string("] binop is unknown, and thus has no precedence to parse against"),
    			loc);
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
    		
    		// an identifer can be followed by a bind or an assignment, or can be a basic variable reference
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
    		
    		Outcome<std::unique_ptr<Ast>, Error> result(ParseAffix(env));
    		
    		if (!result)
    			return Outcome<std::unique_ptr<Ast>, Error>(result.GetTwo());
    			
    		if (tok != Token::RParen)
    		{
    			Error error(Error::Kind::Syntax, "Missing Closing Parenthesis", loc);
    			Outcome<std::unique_ptr<Ast>, Error> result(error);
    			return Outcome<std::unique_ptr<Ast>, Error>(result.GetTwo());
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
    		Error error(Error::Kind::Syntax, "Unknown Basic Token:" + TokenToString(tok), loc);
    		return Outcome<std::unique_ptr<Ast>, Error>(error);
    	}
    	} // !switch(tok)
    }
    
    
    /*  block = '{' affix (';' affix (';')?)* '}'
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
    	Outcome<std::unique_ptr<Ast>, Error> result(Error(Error::Kind::Default, "Default Error", loc));
    	Location left_loc = loc;

      if (tok != Token::LBrace)
		  {
			  Error error(Error::Kind::Syntax, "Expected '{' to begin a block", loc);
			  return Outcome<std::unique_ptr<Ast>, Error>(error);
		  }
		
		  nexttok(); // eat '{'
    	
    	do {
        // eat the semicolon that caused this to loop.
    		if (tok == Token::Semicolon)
    		{
    			nexttok(); // eat ';'
    		}
    		// this isn't an else if to catch the case where we have a 
    		// statment which is followed by a semicolon which ends the 
    		// block. "x := 1; x + 1;" and "x := 1; x + 1" should both 
    		// end the block. whereas "x := 1 x + 1" would be an error,
    		// however when we add application terms, this would parse 
    		// as applying '1' and passing in the argument 'x' which 
    		// would be an application error, as integers cannot be applied.
    		// inside the body of a function, we parse a full term, which 
    		// means that terms can end with an EOF or a '}'
    		if (tok == Token::End || tok == Token::RBrace)
    		{
    			break;
    		}
    		
    		// parse a statement
    		result = ParseAffix(env);
    		
    		if (!result)
    			return Outcome<std::unique_ptr<Ast>, Error>(result.GetTwo());
    		
    		// add it to the current block
    		stmnts.emplace_back(std::move(result.GetOne()));
    	} while (tok == Token::Semicolon);
    	
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
    		Error error(Error::Kind::Syntax, "Functions must begin with 'fn'", loc);
    		return Outcome<std::unique_ptr<Ast>, Error>(error);
    	}
    	
    	nexttok(); // eat 'fn'
    	
    	
    	if (tok != Token::Id)
    	{
    		Error error(Error::Kind::Syntax, "Functions must have a name", loc);
    		return Outcome<std::unique_ptr<Ast>, Error>(error);
    	}
    	
    	name = env->symbols->Intern(txt); // intern the functions name
		
		nexttok(); // eat 'Id'
		
		if (tok != Token::LParen)
		{
			Error error(Error::Kind::Syntax, "Expected '(' for the argument list", loc);
			return Outcome<std::unique_ptr<Ast>, Error>(error);
		}
		
		nexttok(); // eat '('
		
		if (tok == Token::Id) // beginning of the argument list
		{
			do {
				if (tok == Token::Comma)
				{
					nexttok(); // eat ','
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
			Error error(Error::Kind::Syntax, "Expected ')' to end the argument list", loc);
			return Outcome<std::unique_ptr<Ast>, Error>(error);
		}
		
		nexttok(); // eat ')'
		
	
		
		// parse the body of the function.
		Outcome<std::unique_ptr<Ast>, Error> body_res = ParseBlock(env);
		
		if (!body_res)
		{
			return Outcome<std::unique_ptr<Ast>, Error>(body_res.GetTwo());
		}
		
		if (tok != Token::RBrace)
		{
			Error error(Error::Kind::Syntax, "Expected '}' to end the function body", loc);
			return Outcome<std::unique_ptr<Ast>, Error>(error);
		}
		
		nexttok(); // eat '}'
		
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
    		Error error(Error::Kind::Syntax, "Expected a Identifier for the argument", loc);
    		return Outcome<std::pair<InternedString, Type*>, Error>(error);
    	}
    	
    	name = env->symbols->Intern(txt);
    	
    	nexttok(); // eat 'Id'
    	
    	if (tok != Token::Colon)
    	{
    		Error error(Error::Kind::Syntax, "Expected a ':' for the arguments type annotation", loc);
    		return Outcome<std::pair<InternedString, Type*>, Error>(error);
    	}
    	
    	nexttok(); // eat ':'
    	
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
    		Error error(Error::Kind::Syntax, "Unknown Type Token:" + TokenToString(tok), loc);
    		return Outcome<Type*, Error>(error);
    	}
    	}
    }
        
}
