// Copyright (C) 2023 cadence
// 
// This file is part of pink.
// 
// pink is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// pink is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with pink.  If not, see <http://www.gnu.org/licenses/>.

#include "front/Token.h"

#include "aux/Error.h"

#include "support/FatalError.h"

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
  case Token::Pointer: {
    return {"Token::Pointer"};
  }
  case Token::Slice: {
    return {"Token::Slice"};
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
