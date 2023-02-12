/* Generated by re2c 3.0 on Sat Feb 11 20:18:57 2023 */
#line 1 "source/front/Lexer.re"
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

  buffer += txt;

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
#line 113 "source/front/Lexer.re"


// NOLINTBEGIN(cppcoreguidelines-avoid-goto)
// #REASON: re2c uses gotos to implement the lexer and as all of the
// gotos are from generated code we are trusting re2c to
// use gotos in a safe and sane way here.
auto Lexer::lex() -> Token {
  while (true) {
    token = cursor;

    
#line 110 "source/front/Lexer.cpp"
{
	char yych;
	yych = *cursor;
	switch (yych) {
		case '\t':
		case '\n':
		case ' ': goto yy2;
		case '!':
		case '$':
		case '%':
		case '&':
		case '*':
		case '+':
		case '-':
		case '/':
		case '<':
		case '>':
		case '@':
		case '^':
		case '|':
		case '~': goto yy4;
		case '(': goto yy6;
		case ')': goto yy7;
		case ',': goto yy8;
		case '.': goto yy9;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': goto yy10;
		case ':': goto yy12;
		case ';': goto yy14;
		case '=': goto yy15;
		case 'A':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'O':
		case 'Q':
		case 'R':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
		case '_':
		case 'a':
		case 'b':
		case 'c':
		case 'g':
		case 'h':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 'u':
		case 'x':
		case 'y':
		case 'z': goto yy17;
		case 'B': goto yy20;
		case 'I': goto yy21;
		case 'N': goto yy22;
		case 'P': goto yy23;
		case 'S': goto yy24;
		case '[': goto yy25;
		case ']': goto yy26;
		case 'd': goto yy27;
		case 'e': goto yy28;
		case 'f': goto yy29;
		case 'i': goto yy30;
		case 'n': goto yy31;
		case 't': goto yy32;
		case 'v': goto yy33;
		case 'w': goto yy34;
		case '{': goto yy35;
		case '}': goto yy36;
		default:
			if (end <= cursor) goto yy101;
			goto yy1;
	}
yy1:
	++cursor;
#line 159 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::Error; }
#line 214 "source/front/Lexer.cpp"
yy2:
	yych = *++cursor;
	switch (yych) {
		case '\t':
		case '\n':
		case ' ': goto yy2;
		default: goto yy3;
	}
yy3:
#line 158 "source/front/Lexer.re"
	{ UpdateLocation(); continue; }
#line 226 "source/front/Lexer.cpp"
yy4:
	yych = *++cursor;
	switch (yych) {
		case '!':
		case '$':
		case '%':
		case '&':
		case '*':
		case '+':
		case '-':
		case '/':
		case '<':
		case '=':
		case '>':
		case '@':
		case '^':
		case '|':
		case '~': goto yy4;
		default: goto yy5;
	}
yy5:
#line 155 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::Op; }
#line 250 "source/front/Lexer.cpp"
yy6:
	++cursor;
#line 147 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::LParen; }
#line 255 "source/front/Lexer.cpp"
yy7:
	++cursor;
#line 148 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::RParen; }
#line 260 "source/front/Lexer.cpp"
yy8:
	++cursor;
#line 142 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::Comma; }
#line 265 "source/front/Lexer.cpp"
yy9:
	++cursor;
#line 141 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::Dot; }
#line 270 "source/front/Lexer.cpp"
yy10:
	yych = *++cursor;
	switch (yych) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': goto yy10;
		default: goto yy11;
	}
yy11:
#line 156 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::Integer; }
#line 289 "source/front/Lexer.cpp"
yy12:
	yych = *++cursor;
	switch (yych) {
		case '=': goto yy37;
		default: goto yy13;
	}
yy13:
#line 144 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::Colon; }
#line 299 "source/front/Lexer.cpp"
yy14:
	++cursor;
#line 143 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::Semicolon;}
#line 304 "source/front/Lexer.cpp"
yy15:
	yych = *++cursor;
	switch (yych) {
		case '!':
		case '$':
		case '%':
		case '&':
		case '*':
		case '+':
		case '-':
		case '/':
		case '<':
		case '=':
		case '>':
		case '@':
		case '^':
		case '|':
		case '~': goto yy4;
		default: goto yy16;
	}
yy16:
#line 145 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::Equals; }
#line 328 "source/front/Lexer.cpp"
yy17:
	yych = *++cursor;
yy18:
	switch (yych) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
		case '_':
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z': goto yy17;
		default: goto yy19;
	}
yy19:
#line 154 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::Id; }
#line 401 "source/front/Lexer.cpp"
yy20:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'o': goto yy38;
		default: goto yy18;
	}
yy21:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'n': goto yy39;
		default: goto yy18;
	}
yy22:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'i': goto yy40;
		default: goto yy18;
	}
yy23:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'o': goto yy41;
		default: goto yy18;
	}
yy24:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'l': goto yy42;
		default: goto yy18;
	}
yy25:
	++cursor;
#line 151 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::LBracket; }
#line 441 "source/front/Lexer.cpp"
yy26:
	++cursor;
#line 152 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::RBracket; }
#line 446 "source/front/Lexer.cpp"
yy27:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'o': goto yy43;
		default: goto yy18;
	}
yy28:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'l': goto yy45;
		default: goto yy18;
	}
yy29:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'a': goto yy46;
		case 'n': goto yy47;
		default: goto yy18;
	}
yy30:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'f': goto yy49;
		default: goto yy18;
	}
yy31:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'i': goto yy51;
		default: goto yy18;
	}
yy32:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'h': goto yy52;
		case 'r': goto yy53;
		default: goto yy18;
	}
yy33:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'a': goto yy54;
		default: goto yy18;
	}
yy34:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'h': goto yy55;
		default: goto yy18;
	}
yy35:
	++cursor;
#line 149 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::LBrace; }
#line 509 "source/front/Lexer.cpp"
yy36:
	++cursor;
#line 150 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::RBrace; }
#line 514 "source/front/Lexer.cpp"
yy37:
	++cursor;
#line 146 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::ColonEq; }
#line 519 "source/front/Lexer.cpp"
yy38:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'o': goto yy56;
		default: goto yy18;
	}
yy39:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 't': goto yy57;
		default: goto yy18;
	}
yy40:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'l': goto yy58;
		default: goto yy18;
	}
yy41:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'i': goto yy60;
		default: goto yy18;
	}
yy42:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'i': goto yy61;
		default: goto yy18;
	}
yy43:
	yych = *++cursor;
	switch (yych) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
		case '_':
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z': goto yy17;
		default: goto yy44;
	}
yy44:
#line 139 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::Do; }
#line 626 "source/front/Lexer.cpp"
yy45:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 's': goto yy62;
		default: goto yy18;
	}
yy46:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'l': goto yy63;
		default: goto yy18;
	}
yy47:
	yych = *++cursor;
	switch (yych) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
		case '_':
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z': goto yy17;
		default: goto yy48;
	}
yy48:
#line 133 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::Fn; }
#line 712 "source/front/Lexer.cpp"
yy49:
	yych = *++cursor;
	switch (yych) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
		case '_':
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z': goto yy17;
		default: goto yy50;
	}
yy50:
#line 135 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::If; }
#line 784 "source/front/Lexer.cpp"
yy51:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'l': goto yy64;
		default: goto yy18;
	}
yy52:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'e': goto yy66;
		default: goto yy18;
	}
yy53:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'u': goto yy67;
		default: goto yy18;
	}
yy54:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'r': goto yy68;
		default: goto yy18;
	}
yy55:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'i': goto yy70;
		default: goto yy18;
	}
yy56:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'l': goto yy71;
		default: goto yy18;
	}
yy57:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'e': goto yy72;
		default: goto yy18;
	}
yy58:
	yych = *++cursor;
	switch (yych) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
		case '_':
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z': goto yy17;
		default: goto yy59;
	}
yy59:
#line 125 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::NilType; }
#line 905 "source/front/Lexer.cpp"
yy60:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'n': goto yy73;
		default: goto yy18;
	}
yy61:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'c': goto yy74;
		default: goto yy18;
	}
yy62:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'e': goto yy75;
		default: goto yy18;
	}
yy63:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 's': goto yy77;
		default: goto yy18;
	}
yy64:
	yych = *++cursor;
	switch (yych) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
		case '_':
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z': goto yy17;
		default: goto yy65;
	}
yy65:
#line 124 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::Nil; }
#line 1005 "source/front/Lexer.cpp"
yy66:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'n': goto yy78;
		default: goto yy18;
	}
yy67:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'e': goto yy80;
		default: goto yy18;
	}
yy68:
	yych = *++cursor;
	switch (yych) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
		case '_':
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z': goto yy17;
		default: goto yy69;
	}
yy69:
#line 134 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::Var; }
#line 1091 "source/front/Lexer.cpp"
yy70:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'l': goto yy82;
		default: goto yy18;
	}
yy71:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'e': goto yy83;
		default: goto yy18;
	}
yy72:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'g': goto yy84;
		default: goto yy18;
	}
yy73:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 't': goto yy85;
		default: goto yy18;
	}
yy74:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'e': goto yy86;
		default: goto yy18;
	}
yy75:
	yych = *++cursor;
	switch (yych) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
		case '_':
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z': goto yy17;
		default: goto yy76;
	}
yy76:
#line 137 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::Else; }
#line 1198 "source/front/Lexer.cpp"
yy77:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'e': goto yy88;
		default: goto yy18;
	}
yy78:
	yych = *++cursor;
	switch (yych) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
		case '_':
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z': goto yy17;
		default: goto yy79;
	}
yy79:
#line 136 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::Then; }
#line 1277 "source/front/Lexer.cpp"
yy80:
	yych = *++cursor;
	switch (yych) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
		case '_':
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z': goto yy17;
		default: goto yy81;
	}
yy81:
#line 127 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::True; }
#line 1349 "source/front/Lexer.cpp"
yy82:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'e': goto yy90;
		default: goto yy18;
	}
yy83:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'a': goto yy92;
		default: goto yy18;
	}
yy84:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'e': goto yy93;
		default: goto yy18;
	}
yy85:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'e': goto yy94;
		default: goto yy18;
	}
yy86:
	yych = *++cursor;
	switch (yych) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
		case '_':
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z': goto yy17;
		default: goto yy87;
	}
yy87:
#line 131 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::Slice; }
#line 1449 "source/front/Lexer.cpp"
yy88:
	yych = *++cursor;
	switch (yych) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
		case '_':
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z': goto yy17;
		default: goto yy89;
	}
yy89:
#line 128 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::False; }
#line 1521 "source/front/Lexer.cpp"
yy90:
	yych = *++cursor;
	switch (yych) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
		case '_':
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z': goto yy17;
		default: goto yy91;
	}
yy91:
#line 138 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::While; }
#line 1593 "source/front/Lexer.cpp"
yy92:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'n': goto yy95;
		default: goto yy18;
	}
yy93:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'r': goto yy97;
		default: goto yy18;
	}
yy94:
	yych = *++cursor;
	switch (yych) {
		case 0x00: goto yy19;
		case 'r': goto yy99;
		default: goto yy18;
	}
yy95:
	yych = *++cursor;
	switch (yych) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
		case '_':
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z': goto yy17;
		default: goto yy96;
	}
yy96:
#line 129 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::BooleanType; }
#line 1686 "source/front/Lexer.cpp"
yy97:
	yych = *++cursor;
	switch (yych) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
		case '_':
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z': goto yy17;
		default: goto yy98;
	}
yy98:
#line 126 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::IntegerType; }
#line 1758 "source/front/Lexer.cpp"
yy99:
	yych = *++cursor;
	switch (yych) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
		case '_':
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z': goto yy17;
		default: goto yy100;
	}
yy100:
#line 130 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::Pointer; }
#line 1830 "source/front/Lexer.cpp"
yy101:
#line 160 "source/front/Lexer.re"
	{ UpdateLocation(); return Token::End; }
#line 1834 "source/front/Lexer.cpp"
}
#line 161 "source/front/Lexer.re"

  }
}
// NOLINTEND(cppcoreguidelines-avoid-goto)
} // namespace pink
