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

#include <iterator> // std::distance

#include "front/Lexer.h"

namespace pink {
Lexer::Lexer()
    : location(1, 0, 1, 0) {
  end = cursor = marker = token = buffer.end();
}

Lexer::Lexer(std::string_view text)
    : location(1, 0, 1, 0),
      buffer(text) {
  end    = buffer.end();
  cursor = marker = token = buffer.begin();
}

void Lexer::SetBuffer(std::string_view text) {
  buffer = text;
  end    = buffer.end();
  cursor = marker = token = buffer.begin();
}

void Lexer::AppendToBuffer(std::string_view txt) {
  auto cursor_dist = std::distance(buffer.begin(), cursor);
  auto marker_dist = std::distance(buffer.begin(), marker);
  auto token_dist  = std::distance(buffer.begin(), token);

  buffer.append(txt);

  end    = buffer.end();
  cursor = buffer.begin() + cursor_dist;
  marker = buffer.begin() + marker_dist;
  token  = buffer.begin() + token_dist;
}

void Lexer::Reset() {
  location = {1, 0, 1, 0};
  buffer.clear();
  end = cursor = marker = token = buffer.end();
}

auto Lexer::EndOfInput() const -> bool { return (end - cursor) == 0; }

void Lexer::UpdateLocation() {
  auto length = cursor - token;

  location.firstLine   = location.lastLine;
  location.firstColumn = location.lastColumn;

  for (long i = 0; i < length; i++) {
    if (token[i] == '\n') {
      location.lastLine   += 1;
      location.lastColumn = location.firstColumn = 0;
    } else {
      location.lastColumn += 1;
    }
  }
}

/*
    token points to the beginning of the
    current token being lexed, and cursor points
    to the current position of the lexer,
    so the last token that was lexed is sitting
    between those two positions.

    Therefore we can use the string constructor
    taking a two iterators to construct the
    string from the characters from between that range.
*/
auto Lexer::txt() -> std::string_view { return {token, cursor}; }

auto Lexer::loc() -> Location { return location; }

/*
    These are the definitions of the parsing
    primitives that re2c uses, such that we
    can interoperate between c++ and re2c

    #TODO: i think this regex will allow for identifiers
            like: this-is-an-ident, follow-with-hyphen
            but parse identifierss like:
                    -unop-application-of-an-identifier,
                    binop-application-of-an-identifier- more-text

    hyphen-id = id ('-' id)+;

    #TODO and this regex will allow us to lex
    fully qualified identifiers when we want to
    add namespaces to the language.

    full-id = id ("::" id)+;
*/
/*!re2c
    re2c:api:style = free-form;
    re2c:yyfill:enable = 0;
    re2c:eof = 0;

    re2c:define:YYCTYPE  = "char";
    re2c:define:YYCURSOR = "cursor";
    re2c:define:YYMARKER = "marker";
    re2c:define:YYLIMIT = "end";
    re2c:define:YYPEEK   = "(*cursor);";
    re2c:define:YYSKIP   = "(cursor++);";
    re2c:define:YYBACKUP = "(marker = cursor);";
    re2c:define:YYRESTORE = "(cursor = marker);";
    re2c:define:YYLESSTHAN = "(end > (end - cursor));";

    id=[a-zA-Z_][a-zA-Z0-9_]*;
    int=[0-9]+;
*/

// NOLINTBEGIN(cppcoreguidelines-avoid-goto)
// #REASON: re2c uses gotos to implement the lexer and as all of the
// gotos are from generated code we are trusting re2c to
// use gotos in a safe and sane way here.
auto Lexer::lex() -> Token {
  while (true) {
    token = cursor;

    /*!re2c
        "nil"     { UpdateLocation(); return Token::Nil; }
        "Nil"     { UpdateLocation(); return Token::NilType; }
        "Integer" { UpdateLocation(); return Token::IntegerType; }
        "true"    { UpdateLocation(); return Token::True; }
        "false"   { UpdateLocation(); return Token::False; }
        "Boolean" { UpdateLocation(); return Token::BooleanType; }

        "fn"	{ UpdateLocation(); return Token::Fn; }
        "var"   { UpdateLocation(); return Token::Var; }
        "if"    { UpdateLocation(); return Token::If; }
        "then"  { UpdateLocation(); return Token::Then; }
        "else"  { UpdateLocation(); return Token::Else; }
        "while" { UpdateLocation(); return Token::While; }
        "do"    { UpdateLocation(); return Token::Do; }

        "+"     { UpdateLocation(); return Token::Add; }
        "-"     { UpdateLocation(); return Token::Sub; }
        "*"     { UpdateLocation(); return Token::Star; }
        "/"     { UpdateLocation(); return Token::Divide; }
        "%"     { UpdateLocation(); return Token::Modulo; }
        "&"     { UpdateLocation(); return Token::And; }
        "|"     { UpdateLocation(); return Token::Or; }
        "!"     { UpdateLocation(); return Token::Not; }
        "=="    { UpdateLocation(); return Token::Equals; }
        "!="    { UpdateLocation(); return Token::NotEquals; }
        "<"     { UpdateLocation(); return Token::LessThan; }
        "<="    { UpdateLocation(); return Token::LessThanOrEqual; }
        ">"     { UpdateLocation(); return Token::GreaterThan; }
        ">="    { UpdateLocation(); return Token::GreaterThanOrEqual; }

        "."     { UpdateLocation(); return Token::Dot; }
        ","		  { UpdateLocation(); return Token::Comma; }
        ";"		  { UpdateLocation(); return Token::Semicolon;}
        ":"     { UpdateLocation(); return Token::Colon; }
        "="     { UpdateLocation(); return Token::Assign; }
        ":="    { UpdateLocation(); return Token::ColonEq; }
        "("     { UpdateLocation(); return Token::LParen; }
        ")"     { UpdateLocation(); return Token::RParen; }
        "{"     { UpdateLocation(); return Token::LBrace; }
        "}"	  	{ UpdateLocation(); return Token::RBrace; }
        "["     { UpdateLocation(); return Token::LBracket; }
        "]"     { UpdateLocation(); return Token::RBracket; }
        "->"    { UpdateLocation(); return Token::RArrow; }

        id      { UpdateLocation(); return Token::Id; }
        int     { UpdateLocation(); return Token::Integer; }

        [ \t\n]+ { UpdateLocation(); continue; } // Whitespace
        *        { UpdateLocation(); return Token::Error; } // Unknown Token
        $        { UpdateLocation(); return Token::End; } // End of Input
    */
  }
}
// NOLINTEND(cppcoreguidelines-avoid-goto)
} // namespace pink
