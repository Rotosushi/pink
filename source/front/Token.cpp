#include "front/Token.h"
#include "aux/Error.h"

namespace pink {
/*
        Error reporting/Status Dumping subroutine for
        examining the state of the Lexer/Parser
*/
auto TokenToString(const Token token) -> std::string {
  switch (token) {
  case Token::Error: {
    return {"Token::Error"};
  }
  case Token::End: {
    return {"Token::End"};
  }
  case Token::Id: {
    return {"Token::Id"};
  }
  case Token::Op: {
    return {"Token::Op"};
  }

  case Token::Dot: {
    return {"Token::Dot"};
  }
  case Token::Comma: {
    return {"Token::Comma"};
  }
  case Token::Semicolon: {
    return {"Token::Semicolon"};
  }
  case Token::Colon: {
    return {"Token::Colon"};
  }
  case Token::Equals: {
    return {"Token::Equals"};
  }
  case Token::ColonEq: {
    return {"Token::ColonEq"};
  }
  case Token::LParen: {
    return {"Token::LParen"};
  }
  case Token::RParen: {
    return {"Token::RParen"};
  }
  case Token::LBrace: {
    return {"Token::LBrace"};
  }
  case Token::RBrace: {
    return {"Token::RBrace"};
  }
  case Token::LBracket: {
    return {"Token::LBracket"};
  }
  case Token::RBracket: {
    return {"Token::RBracket"};
  }

  case Token::Nil: {
    return {"Token::Nil"};
  }
  case Token::NilType: {
    return {"Token::NilType"};
  }
  case Token::Integer: {
    return {"Token::Integer"};
  }
  case Token::IntegerType: {
    return {"Token::IntegerType"};
  }
  case Token::True: {
    return {"Token::True"};
  }
  case Token::False: {
    return {"Token::False"};
  }
  case Token::BooleanType: {
    return {"Token::BooleanType"};
  }
  case Token::Ptr: {
    return {"Token::Ptr"};
  }

  case Token::Fn: {
    return {"Token::Fn"};
  }
  case Token::Var: {
    return {"Token::Var"};
  }
  case Token::If: {
    return {"Token::If"};
  }
  case Token::Then: {
    return {"Token::Then"};
  }
  case Token::Else: {
    return {"Token::Else"};
  }
  case Token::While: {
    return {"Token::While"};
  }
  case Token::Do: {
    return {"Token::Do"};
  }
  default: {
    FatalError("Unknown Token Kind", __FILE__, __LINE__);
    return {"Unknown"};
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
auto TokenToBool(const Token token) -> bool {
  switch (token) {
  case Token::Error:
    return false;
  default:
    return true;
  }
}
} // namespace pink
