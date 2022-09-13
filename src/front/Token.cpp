#include "aux/Error.h"
#include "front/Token.h"

namespace pink {
	/*
		Error reporting/Status Dumping subroutine for 
		examining the state of the Lexer/Parser
	*/
    std::string TokenToString(Token t)
    {
        switch (t)
        {
            case Token::Error:    { return std::string("Token::Error");    }
            case Token::End:      { return std::string("Token::End");      }
            case Token::Id:       { return std::string("Token::Id");       }
            case Token::FullyQualifiedId: { return std::string("Token::FullyQualifiedId"); }
            case Token::Op:       { return std::string("Token::Op");       }

            case Token::Dot:      { return std::string("Token::Dot");      }
            case Token::Comma:	  { return std::string("Token::Comma");    }
            case Token::Semicolon:{ return std::string("Token::Semicolon");}
            case Token::Colon:    { return std::string("Token::Colon");    }
            case Token::Equals:   { return std::string("Token::Equals");   }
      			case Token::ColonEq:  { return std::string("Token::ColonEq");  }
            case Token::LParen:   { return std::string("Token::LParen");   }
            case Token::RParen:   { return std::string("Token::RParen");   }
            case Token::LBrace:	  { return std::string("Token::LBrace");   }
            case Token::RBrace:   { return std::string("Token::RBrace");   }
            case Token::LBracket: { return std::string("Token::LBracket"); }
            case Token::RBracket: { return std::string("Token::RBracket"); }

            case Token::Nil:      { return std::string("Token::Nil");      }
            case Token::NilType:  { return std::string("Token::NilType");  }
            case Token::Int:      { return std::string("Token::Int");      }
            case Token::IntType:  { return std::string("Token::IntType");  }
            case Token::True:     { return std::string("Token::True");     }
            case Token::False:    { return std::string("Token::False");    }
            case Token::BoolType: { return std::string("Token::BoolType"); }
            case Token::Fn:		    { return std::string("Token::Fn");       }
            case Token::If:       { return std::string("Token::If");       }
            case Token::Then:     { return std::string("Token::Then");     }
            case Token::Else:     { return std::string("Token::Else");     }
            case Token::While:    { return std::string("Token::While");    }
            case Token::Do:       { return std::string("Token::Do");       }
            default:
            {
                FatalError("Unknown Token Kind", __FILE__, __LINE__);
                return std::string("Unknown");
            }
        }
    }
    
    /*
    	Only returns false on Token::Error,
    	every other token is considered a 
    	valid token. This function mainly 
    	exists so that we can use an assignment 
    	statement within the while conditions 
    	within Parser::ParseInfix. which are there 
    	so that we are peeking the next token 
    	while we parse an infix expression.
    */
    bool TokenToBool(Token t)
    {
    	switch (t)
    	{
    	case Token::Error:
    		return false;
    	default:
    		return true;
    	}
    }
}
