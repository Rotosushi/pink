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
        : lexer()
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

    Outcome<Ast*, Error> Parser::Parse(std::string str, Environment& env)
    {
        lexer.SetBuf(str);
        nexttok(); // prime the lexers
        Outcome<Ast*, Error> result (ParseTerm(env));
        return result;
    }

    Outcome<Ast*, Error> Parser::ParseTerm(Environment& env)
    {
        Outcome<Ast*, Error> result (ParseAffix(env));
        return result;
    }

    Outcome<Ast*, Error> Parser::ParseAffix(Environment& env)
    {
        Location left_loc = loc; // save the location of the lhs of this affix expr
		Outcome<Ast*, Error> left(ParseBasic(env)); // parse the initial term
		
		if (!left) // if the previous parse failed, return the error immediately
			return left;
		
        if (tok == Token::Op) // we assume this is a binary operator appearing after a basic term
        {
        	// Handle the entire binop expression with the Infix Parser
            Outcome<Ast*, Error> result(ParseInfix(left.GetOne(), 0, env));
            return result; // pass or fail, return the result of parsing a binop expression.
        }
        
		// the single term we parsed at the beginning is the result of this parse.
		return left;
    }
    
    
    /*
    	This is an implementation of an operator precedence parser
    	going from this pseudo-code:
    	https://en.wikipedia.org/wiki/Operator-precedence_parser
    */
    Outcome<Ast*, Error> Parser::ParseInfix(Ast* lhs, Precedence precedence, Environment& env)
    {
    	Outcome<Ast*, Error> result(lhs);
    	Token peek_tok;
    	InternedString peek_str;
    	llvm::Optional<std::pair<InternedString, BinopLiteral*>> peek_opt;
    	BinopLiteral* peek_lit;
    	
    	while (TokenToBool(peek_tok = tok) && (peek_tok == Token::Op)  
    		&& (peek_str = env.operators.Intern(txt))
    		&& (peek_opt = env.binops.Lookup(peek_str)) && (peek_opt.hasValue()) 
    		&& (peek_lit = peek_opt->second) && (peek_lit->precedence >= precedence))
    	{
    		InternedString op_str = peek_str;
    		BinopLiteral*  op_lit = peek_lit;
    		Location       op_loc = loc;
    		
    		nexttok(); // eat the operator, Token::Op 
    		
    		Outcome<Ast*, Error> rhs = ParseBasic(env);
    		
    		if (!rhs)
    			return rhs; 
    			
    		while (TokenToBool(peek_tok = tok) && (peek_tok == Token::Op) 
    			&& (peek_str = env.operators.Intern(txt))
    			&& (peek_opt = env.binops.Lookup(peek_str)) && (peek_opt.hasValue())
    			&& (peek_lit = peek_opt->second) 
    			&& ((peek_lit->precedence > op_lit->precedence) 
    				|| ((peek_lit->associativity == Associativity::Right)
    				&& (peek_lit->precedence == op_lit->precedence))))
    		{
    			rhs = ParseInfix(rhs.GetOne(), peek_lit->precedence, env);
    			
    			if (!rhs)
    				return rhs; 
    		}
    		
    		Location binop_loc(op_loc.firstLine, op_loc.firstColumn, loc.firstLine, loc.firstColumn);
    		result = Outcome<Ast*, Error>(new Binop(binop_loc, op_str, result.GetOne(),  rhs.GetOne()));
    	}
    	return result;
    }
    
    
    Outcome<Ast*, Error> Parser::ParseBasic(Environment& env)
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
    			
    			Outcome<Ast*, Error> rhs(ParseAffix(env)); // parse the right hand side of the binding
    			
    			if (!rhs)
    				return rhs;
    				
    			Location bind_loc(lhs_loc.firstLine, lhs_loc.firstColumn, loc.firstLine, loc.firstColumn);
    			Outcome<Ast*, Error> result(new Bind(bind_loc, id, rhs.GetOne()));
    			return result;
    		}
    		else if (tok == Token::Equals) // an assignment expression
        	{
		        nexttok(); // eat '='

		        Outcome<Ast*, Error> rhs(ParseTerm(env));
		        
		        if (!rhs)
		        	return rhs;
		        
		        // loc holds the location of the rhs of the term after the above call to ParseTerm
		        Location assign_loc(lhs_loc.firstLine, lhs_loc.firstColumn, loc.firstLine, loc.firstColumn);
		        Outcome<Ast*, Error> result(new Assignment(assign_loc, new Variable(lhs_loc, id), rhs.GetOne()));
		        return result;
        	}
        	else // this identifer was simply a variable reference 
        	{
        		Outcome<Ast*, Error> result(new Variable(lhs_loc, id));
        		return result;
        	}
        	break;
    	}
    	
    	case Token::Op: // an operator appearing in the basic position is a unop.
    	{
    		Location lhs_loc = loc; // save the lhs location
    		InternedString op = env.operators.Intern(txt);
    		
    		nexttok(); // eat the op
    		
    		// unops all bind to their immediate right hand side
    		Outcome<Ast*, Error> rhs(ParseBasic(env));    	
    		
    		if (!rhs)
    			return rhs;
    			
    		Location unop_loc(lhs_loc.firstLine, lhs_loc.firstColumn, loc.firstLine, loc.firstColumn);
    		Outcome<Ast*, Error> result(new Unop(unop_loc, op, rhs.GetOne()));
    		return result;
    	}
    	
    	case Token::LParen:
    	{
    		nexttok(); // eat the '('
    		
    		Outcome<Ast*, Error> result(ParseAffix(env));
    		
    		if (!result)
    			return result;
    			
    		if (tok != Token::RParen)
    		{
    			Outcome<Ast*, Error> error(Error(Error::Kind::Syntax, "Missing Closing Parenthesis", loc));
    			return error;
    		}
    		else 
    		{
    			nexttok(); // eat the ')'
    			return result;
    		}
    	}
    	
    	case Token::Nil:
    	{	
    		Location lhs_loc = loc;
    		nexttok(); // eat 'nil'
    		Outcome<Ast*, Error> result(new Nil(lhs_loc));
    		return result;
    	}
    	
      
        case Token::Int:
        {
        	Location lhs_loc = loc;
        	int value = std::stoi(txt);
        	nexttok(); // eat '42'
        	Outcome<Ast*, Error> result(new Int(lhs_loc, value));
        	return result;
        }
        
        case Token::True:
        {
        	Location lhs_loc = loc;
        	nexttok(); // Eat "true"
        	Outcome<Ast*, Error> result(new Bool(lhs_loc, true));
        	return result;
        }
        
        case Token::False:
        {
        	Location lhs_loc = loc;
        	nexttok(); // Eat "false"
        	Outcome<Ast*, Error> result(new Bool(lhs_loc, false));
        	return result;
        }
        
    	default:
    	{
    		return Outcome<Ast*, Error>(Error(Error::Kind::Syntax, "Unknown Basic Token:" + TokenToString(tok), loc));
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
    		return Outcome<Type*, Error>(Error(Error::Kind::Syntax, "Unknown Type Token:" + TokenToString(tok), loc));
    	}
    	}
    }
        
}
