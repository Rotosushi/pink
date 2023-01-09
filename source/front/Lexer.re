#include <iterator> // std::distance

#include "front/Lexer.h"

namespace pink {
Lexer::Lexer() : loc(1, 0, 1, 0) {
  end = cursor = marker = token = buffer.end();
}

Lexer::Lexer(std::string &buf) : loc(1, 0, 1, 0), buffer(buf) {
  end = buffer.end();
  cursor = marker = token = buffer.begin();
}

auto Lexer::GetBuf() const -> const std::string & { return buffer; }

void Lexer::SetBuf(std::string &buf) {
  buffer = buf;
  end = buffer.end();
  cursor = marker = token = buffer.begin();
}

void Lexer::AppendBuf(const char *txt) {
  auto cursor_dist = std::distance(buffer.begin(), cursor);
  auto marker_dist = std::distance(buffer.begin(), marker);
  auto token_dist = std::distance(buffer.begin(), token);

  buffer += txt;

  end = buffer.end();
  cursor = buffer.begin() + cursor_dist;
  marker = buffer.begin() + marker_dist;
  token = buffer.begin() + token_dist;
}

void Lexer::AppendBuf(std::string &buf) { AppendBuf(buf.c_str()); }

void Lexer::Getline(std::istream &input) {
  auto cursor_distance = std::distance(buffer.begin(), cursor);
  auto marker_distance = std::distance(buffer.begin(), marker);
  auto token_distance = std::distance(buffer.begin(), token);

  char character = '\0';

  do {
    character = static_cast<char>(input.get());

    if (input.eof()) {
      break;
    }

    buffer += character;
  } while (character != '\n');

  end = buffer.end();
  cursor = buffer.begin() + cursor_distance;
  marker = buffer.begin() + marker_distance;
  token = buffer.begin() + token_distance;
}

void Lexer::Reset() {
  loc = {1, 0, 1, 0};
  buffer.clear();
  end = cursor = marker = token = buffer.end();
}

auto Lexer::EndOfInput() const -> bool { return (end - cursor) == 0; }

void Lexer::UpdateLoc() {
  auto length = cursor - token;

  loc.firstLine = loc.lastLine;
  loc.firstColumn = loc.lastColumn;

  for (long i = 0; i < length; i++) {
    if (token[i] == '\n') {
      loc.lastLine += 1;
      loc.lastColumn = loc.firstColumn = 0;
    } else {
      loc.lastColumn += 1;
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
auto Lexer::yytxt() -> std::string { return {token, cursor}; }

auto Lexer::yyloc() -> Location { return loc; }

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
    fullyQualifiedId=id("::" id)+;
    op=[+*\-/%<=>&|\^!~@$]+;
    int=[0-9]+;
*/

// NOLINTBEGIN(cppcoreguidelines-avoid-goto)
// #REASON: re2c uses gotos to implement the lexer and as all of the
// gotos are from generated code we are trusting re2c to
// use gotos in a safe and sane way here.
auto Lexer::yylex() -> Token {
  while (true) {
    token = cursor;

    /*!re2c
        "nil"   { UpdateLoc(); return Token::Nil; }
        "Nil"   { UpdateLoc(); return Token::NilType; }
        "Integer"   { UpdateLoc(); return Token::IntegerType; }
        "true"  { UpdateLoc(); return Token::True; }
        "false" { UpdateLoc(); return Token::False; }
        "Boolean"  { UpdateLoc(); return Token::BooleanType; }
        "Ptr"   { UpdateLoc(); return Token::Ptr; }

        "fn"	{ UpdateLoc(); return Token::Fn; }
        "var"   { UpdateLoc(); return Token::Var; }
        "if"    { UpdateLoc(); return Token::If; }
        "then"  { UpdateLoc(); return Token::Then; }
        "else"  { UpdateLoc(); return Token::Else; }
        "while" { UpdateLoc(); return Token::While; }
        "do"    { UpdateLoc(); return Token::Do; }

        "."     { UpdateLoc(); return Token::Dot; }
        ","		  { UpdateLoc(); return Token::Comma; }
        ";"		  { UpdateLoc(); return Token::Semicolon;}
        ":"     { UpdateLoc(); return Token::Colon; }
        "="     { UpdateLoc(); return Token::Equals; }
        ":="    { UpdateLoc(); return Token::ColonEq; }
        "("     { UpdateLoc(); return Token::LParen; }
        ")"     { UpdateLoc(); return Token::RParen; }
        "{"     { UpdateLoc(); return Token::LBrace; }
        "}"	  	{ UpdateLoc(); return Token::RBrace; }
        "["     { UpdateLoc(); return Token::LBracket; }
        "]"     { UpdateLoc(); return Token::RBracket; }

        id      { UpdateLoc(); return Token::Id; }
        fullyQualifiedId { UpdateLoc(); return Token::FullyQualifiedId; }
        op      { UpdateLoc(); return Token::Op; }
        int     { UpdateLoc(); return Token::Integer; }

        [ \t\n]+ { UpdateLoc(); continue; } // Whitespace
        *        { UpdateLoc(); return Token::Error; } // Unknown Token
        $        { UpdateLoc(); return Token::End; } // End of Input
    */
  }
}
// NOLINTEND(cppcoreguidelines-avoid-goto)
} // namespace pink
