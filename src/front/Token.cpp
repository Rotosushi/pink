#include "aux/Error.hpp"
#include "front/Token.hpp"

namespace pink {
    std::string TokenToString(Token t)
    {
        switch (t)
        {
            case Token::Error:    { return std::string("Token::Error");    }
            case Token::End:      { return std::string("Token::End");      }
            case Token::Id:       { return std::string("Token::Id");       }
            case Token::Op:       { return std::string("Token::Op");       }
            case Token::Colon:    { return std::string("Token::Colon");    }
            case Token::Equals:   { return std::string("Token::Equals");   }
            case Token::LParen:   { return std::string("Token::LParen");   }
            case Token::RParen:   { return std::string("Token::RParen");   }
            case Token::Nil:      { return std::string("Token::Nil");      }
            case Token::NilType:  { return std::string("Token::NilType");  }
            case Token::Int:      { return std::string("Token::Int");      }
            case Token::IntType:  { return std::string("Token::IntType");  }
            case Token::True:     { return std::string("Token::True");     }
            case Token::False:    { return std::string("Token::False");    }
            case Token::BoolType: { return std::string("Token::BoolType"); }
            default:
            {
                FatalError("Unknown Token Kind", __FILE__, __LINE__);
                return std::string("Unknown");
            }
        }
    }
}
