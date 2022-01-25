#include "front/Parser.h"

#include "aux/Environment.h"

// Syntactic Forms
#include "ast/Variable.h"
#include "ast/Bind.h"
#include "ast/Assignment.h"
#include "ast/Binop.h"
#include "ast/Unop.h"

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
        : lexer(), tok(Token::Error), loc(), txt()
    {

    }

    Parser::~Parser()
    {

    }

    void Parser::nexttok()
    {
        tok = lexer.yylex();
        loc = lexer.yyloc();
        txt = lexer.yytxt();
    }

    Outcome<std::unique_ptr<Ast>, Error> Parser::Parse(std::string str, Environment& env)
    {
    	tok = Token::Error;
    	loc = Location();
    	txt = "";
        lexer.SetBuf(str);
        nexttok(); // prime the lexers
        return ParseTerm(env);
    }

    Outcome<std::unique_ptr<Ast>, Error> Parser::ParseTerm(Environment& env)
    {
        return ParseAffix(env);
    }

    Outcome<std::unique_ptr<Ast>, Error> Parser::ParseAffix(Environment& env)
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
    Outcome<std::unique_ptr<Ast>, Error> Parser::ParseInfix(std::unique_ptr<Ast> lhs, Precedence precedence, Environment& env)
    {
    	Outcome<std::unique_ptr<Ast>, Error> result(std::move(lhs));
    	Token peek_tok;
    	InternedString peek_str = nullptr;
    	llvm::Optional<std::pair<InternedString, BinopLiteral*>> peek_opt;
    	BinopLiteral* peek_lit = nullptr;
    	
    	while (TokenToBool(peek_tok = tok) && (peek_tok == Token::Op)  
    		&& (peek_str = env.operators.Intern(txt))
    		&& (peek_opt = env.binops.Lookup(peek_str)) && (peek_opt.hasValue()) 
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
    			&& (peek_str = env.operators.Intern(txt))
    			&& (peek_opt = env.binops.Lookup(peek_str)) && (peek_opt.hasValue())
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
    
    
    Outcome<std::unique_ptr<Ast>, Error> Parser::ParseBasic(Environment& env)
    {
    	switch(tok)
    	{
    	case Token::Id:
    	{
    		Location lhs_loc = loc; // save the beginning location
    		InternedString id = env.symbols.Intern(txt); // Intern the identifier
    		
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

		        Outcome<std::unique_ptr<Ast>, Error> rhs(ParseTerm(env));
		        
		        if (!rhs)
		        	return Outcome<std::unique_ptr<Ast>, Error>(rhs.GetTwo());
		        
		        // loc holds the location of the rhs of the term after the above call to ParseTerm
		        Location assign_loc(lhs_loc.firstLine, lhs_loc.firstColumn, loc.firstLine, loc.firstColumn);
		        Outcome<std::unique_ptr<Ast>, Error> result(std::make_unique<Assignment>(assign_loc, std::make_unique<Variable>(lhs_loc, id), std::move(rhs.GetOne())));
		        
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
    		InternedString op = env.operators.Intern(txt);
    		
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
        	nexttok(); // eat '42'
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
        
    	default:
    	{
    		Error error(Error::Kind::Syntax, "Unknown Basic Token:" + TokenToString(tok), loc);
    		return Outcome<std::unique_ptr<Ast>, Error>(error);
    	}
    	} // !switch(tok)
    }
    
    
    Outcome<Type*, Error> Parser::ParseBasicType(Environment& env)
    {
    	switch(tok)
    	{
    	case Token::NilType:
        {
        	Location lhs_loc = loc;
        	nexttok(); // eat 'Nil'
        	Outcome<Type*, Error> result(env.types.GetNilType());
        	return result;
        }
        
    	case Token::IntType:
        {
        	Location lhs_loc = loc;
        	nexttok(); // Eat "Int"
        	Outcome<Type*, Error> result(env.types.GetIntType());
        	return result;
        }
        
    	case Token::BoolType:
        {
        	Location lhs_loc = loc;
        	nexttok(); // Eat "Bool"
        	Outcome<Type*, Error> result(env.types.GetBoolType());
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
