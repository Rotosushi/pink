/* Generated by re2c 3.0 on Fri Sep 30 14:48:14 2022 */
#line 1 "src/front/Lexer.re"
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
  // save the relative offsets of each iterator.
  auto cursor_dist = std::distance(buffer.begin(), cursor);
  auto marker_dist = std::distance(buffer.begin(), marker);
  auto token_dist = std::distance(buffer.begin(), token);

  buffer +=
      txt; // perform the append operation, which may invalidate the iterators

  // construct new valid iterators, at their old relative offsets within the new
  // string.
  end = buffer.end();
  cursor = buffer.begin() + cursor_dist;
  marker = buffer.begin() + marker_dist;
  token = buffer.begin() + token_dist;
}

void Lexer::AppendBuf(std::string &buf) {
  // this procedure modifies the buffer we are currently parsing
  // by appending the text passed onto it.
  // normally this is inadvisable, because if the string being
  // appened onto needs to reallocate its internal buffer to
  // accomidate the new characters then any iterators into
  // said buffer will be invalidated, and we will not keep our
  // location within the buffer. this is a problem, because we
  // only want to add new text to lex, we don't want to destroy
  // our position within the array. so we can keep lexing where
  // we left off.
  // to solve this problem we are going to take advantage of the
  // fact that when we perform the append operation, the original
  // text, and position of said text within the new buffer will
  // be identical to what it was before the append operation.
  // this is because the append operation does not modify the
  // existing text, or its relative position within the buffer,
  // it only adds new text after the end.
  // thus if we store the distance that each iterator is into the
  // current buffer, after the append, we can 'restore' the position
  // of the iterators within the new buffer containing the
  // old text and the appended new text.

  // save the relative offsets of each iterator.
  auto cursor_dist = std::distance(buffer.begin(), cursor);
  auto marker_dist = std::distance(buffer.begin(), marker);
  auto token_dist = std::distance(buffer.begin(), token);

  buffer +=
      buf; // perform the append operation, which may invalidate the iterators

  // construct new valid iterators, at their old relative offsets within the new
  // string.
  end = buffer.end();
  cursor = buffer.begin() + cursor_dist;
  marker = buffer.begin() + marker_dist;
  token = buffer.begin() + token_dist;
}

void Lexer::Reset() {
  loc = {1, 0, 1, 0};
  buffer.clear();
  end = cursor = marker = token = buffer.end();
}

auto Lexer::EndOfInput() const -> bool { return (end - cursor) == 0; }

/*
    This routine works great for single line
    inputs, but should be tested against multi-line
    inputs for a more rigourous compiler. #TODO
*/
void Lexer::UpdateLoc() {
  /*
      since token points to the beginning of the
      current token being lexed, and cursor points
      to the current position of the lexer,
      after a token has been lexed from the buffer,
      it sits between [cursor, token], and the
      string iterator operator -() lets us compute
      that distance in chars directly.

      theoretically we could swap string for wstring
      to support unicode. I think this would still
      work, but i'm not 100% sure about that.
  */
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

    # TODO: i think this regex will allow for identifiers
            like: this-is-an-ident, follow-with-hyphen
            but dissallow idents like:
                    -unop-application-not-an-ident,
                    binop-application-not-an-ident- more-text

    id=[a-zA-Z_]([-]?[a-zA-Z0-9_])*;
*/
#line 168 "src/front/Lexer.re"


// NOLINTBEGIN(cppcoreguidelines-avoid-goto)
// #REASON: re2c uses gotos to implement the lexer, as all of the
// gotos are from generated code, we are trusting re2c to
// use gotos in a safe and sane way here.
auto Lexer::yylex() -> Token {
  while (true) {
    token = cursor;

    
#line 164 "src/front/Lexer.cpp"
{
	char yych;
	unsigned int yyaccept = 0;
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
		case 'g':
		case 'h':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'o':
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
		case '[': goto yy23;
		case ']': goto yy24;
		case 'd': goto yy25;
		case 'e': goto yy26;
		case 'f': goto yy27;
		case 'i': goto yy28;
		case 'n': goto yy29;
		case 'p': goto yy30;
		case 't': goto yy31;
		case 'v': goto yy32;
		case 'w': goto yy33;
		case '{': goto yy34;
		case '}': goto yy35;
		default:
			if (end <= cursor) goto yy89;
			goto yy1;
	}
yy1:
	++cursor;
#line 215 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::Error; }
#line 269 "src/front/Lexer.cpp"
yy2:
	yych = *++cursor;
	switch (yych) {
		case '\t':
		case '\n':
		case ' ': goto yy2;
		default: goto yy3;
	}
yy3:
#line 214 "src/front/Lexer.re"
	{ UpdateLoc(); continue; }
#line 281 "src/front/Lexer.cpp"
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
#line 211 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::Op; }
#line 305 "src/front/Lexer.cpp"
yy6:
	++cursor;
#line 202 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::LParen; }
#line 310 "src/front/Lexer.cpp"
yy7:
	++cursor;
#line 203 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::RParen; }
#line 315 "src/front/Lexer.cpp"
yy8:
	++cursor;
#line 196 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::Comma; }
#line 320 "src/front/Lexer.cpp"
yy9:
	++cursor;
#line 195 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::Dot; }
#line 325 "src/front/Lexer.cpp"
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
#line 212 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::Int; }
#line 344 "src/front/Lexer.cpp"
yy12:
	yych = *++cursor;
	switch (yych) {
		case '=': goto yy36;
		default: goto yy13;
	}
yy13:
#line 199 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::Colon; }
#line 354 "src/front/Lexer.cpp"
yy14:
	++cursor;
#line 197 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::Semicolon;
       }
#line 360 "src/front/Lexer.cpp"
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
#line 200 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::Equals; }
#line 384 "src/front/Lexer.cpp"
yy17:
	yyaccept = 0;
	yych = *(marker = ++cursor);
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
		case ':': goto yy37;
		default: goto yy19;
	}
yy19:
#line 209 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::Id; }
#line 459 "src/front/Lexer.cpp"
yy20:
	yyaccept = 0;
	yych = *(marker = ++cursor);
	switch (yych) {
		case 0x00: goto yy19;
		case 'o': goto yy39;
		default: goto yy18;
	}
yy21:
	yyaccept = 0;
	yych = *(marker = ++cursor);
	switch (yych) {
		case 0x00: goto yy19;
		case 'n': goto yy40;
		default: goto yy18;
	}
yy22:
	yyaccept = 0;
	yych = *(marker = ++cursor);
	switch (yych) {
		case 0x00: goto yy19;
		case 'i': goto yy41;
		default: goto yy18;
	}
yy23:
	++cursor;
#line 206 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::LBracket; }
#line 488 "src/front/Lexer.cpp"
yy24:
	++cursor;
#line 207 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::RBracket; }
#line 493 "src/front/Lexer.cpp"
yy25:
	yyaccept = 0;
	yych = *(marker = ++cursor);
	switch (yych) {
		case 0x00: goto yy19;
		case 'o': goto yy42;
		default: goto yy18;
	}
yy26:
	yyaccept = 0;
	yych = *(marker = ++cursor);
	switch (yych) {
		case 0x00: goto yy19;
		case 'l': goto yy44;
		default: goto yy18;
	}
yy27:
	yyaccept = 0;
	yych = *(marker = ++cursor);
	switch (yych) {
		case 0x00: goto yy19;
		case 'a': goto yy45;
		case 'n': goto yy46;
		default: goto yy18;
	}
yy28:
	yyaccept = 0;
	yych = *(marker = ++cursor);
	switch (yych) {
		case 0x00: goto yy19;
		case 'f': goto yy48;
		default: goto yy18;
	}
yy29:
	yyaccept = 0;
	yych = *(marker = ++cursor);
	switch (yych) {
		case 0x00: goto yy19;
		case 'i': goto yy50;
		default: goto yy18;
	}
yy30:
	yyaccept = 0;
	yych = *(marker = ++cursor);
	switch (yych) {
		case 0x00: goto yy19;
		case 't': goto yy51;
		default: goto yy18;
	}
yy31:
	yyaccept = 0;
	yych = *(marker = ++cursor);
	switch (yych) {
		case 0x00: goto yy19;
		case 'h': goto yy52;
		case 'r': goto yy53;
		default: goto yy18;
	}
yy32:
	yyaccept = 0;
	yych = *(marker = ++cursor);
	switch (yych) {
		case 0x00: goto yy19;
		case 'a': goto yy54;
		default: goto yy18;
	}
yy33:
	yyaccept = 0;
	yych = *(marker = ++cursor);
	switch (yych) {
		case 0x00: goto yy19;
		case 'h': goto yy55;
		default: goto yy18;
	}
yy34:
	++cursor;
#line 204 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::LBrace; }
#line 572 "src/front/Lexer.cpp"
yy35:
	++cursor;
#line 205 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::RBrace; }
#line 577 "src/front/Lexer.cpp"
yy36:
	++cursor;
#line 201 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::ColonEq; }
#line 582 "src/front/Lexer.cpp"
yy37:
	yych = *++cursor;
	switch (yych) {
		case ':': goto yy56;
		default: goto yy38;
	}
yy38:
	cursor = marker;
	switch (yyaccept) {
		case 0: goto yy19;
		case 1: goto yy43;
		case 2: goto yy47;
		case 3: goto yy49;
		case 4: goto yy59;
		case 5: goto yy61;
		case 6: goto yy65;
		case 7: goto yy67;
		case 8: goto yy71;
		case 9: goto yy74;
		case 10: goto yy76;
		case 11: goto yy78;
		case 12: goto yy81;
		case 13: goto yy83;
		case 14: goto yy86;
		default: goto yy88;
	}
yy39:
	yyaccept = 0;
	yych = *(marker = ++cursor);
	switch (yych) {
		case 0x00: goto yy19;
		case 'o': goto yy57;
		default: goto yy18;
	}
yy40:
	yyaccept = 0;
	yych = *(marker = ++cursor);
	switch (yych) {
		case 0x00: goto yy19;
		case 't': goto yy58;
		default: goto yy18;
	}
yy41:
	yyaccept = 0;
	yych = *(marker = ++cursor);
	switch (yych) {
		case 0x00: goto yy19;
		case 'l': goto yy60;
		default: goto yy18;
	}
yy42:
	yyaccept = 1;
	yych = *(marker = ++cursor);
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
		case ':':
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
		case 'z': goto yy18;
		default: goto yy43;
	}
yy43:
#line 193 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::Do; }
#line 706 "src/front/Lexer.cpp"
yy44:
	yyaccept = 0;
	yych = *(marker = ++cursor);
	switch (yych) {
		case 0x00: goto yy19;
		case 's': goto yy62;
		default: goto yy18;
	}
yy45:
	yyaccept = 0;
	yych = *(marker = ++cursor);
	switch (yych) {
		case 0x00: goto yy19;
		case 'l': goto yy63;
		default: goto yy18;
	}
yy46:
	yyaccept = 2;
	yych = *(marker = ++cursor);
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
		case ':':
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
		case 'z': goto yy18;
		default: goto yy47;
	}
yy47:
#line 187 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::Fn; }
#line 796 "src/front/Lexer.cpp"
yy48:
	yyaccept = 3;
	yych = *(marker = ++cursor);
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
		case ':':
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
		case 'z': goto yy18;
		default: goto yy49;
	}
yy49:
#line 189 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::If; }
#line 870 "src/front/Lexer.cpp"
yy50:
	yyaccept = 0;
	yych = *(marker = ++cursor);
	switch (yych) {
		case 0x00: goto yy19;
		case 'l': goto yy64;
		default: goto yy18;
	}
yy51:
	yyaccept = 0;
	yych = *(marker = ++cursor);
	switch (yych) {
		case 0x00: goto yy19;
		case 'r': goto yy66;
		default: goto yy18;
	}
yy52:
	yyaccept = 0;
	yych = *(marker = ++cursor);
	switch (yych) {
		case 0x00: goto yy19;
		case 'e': goto yy68;
		default: goto yy18;
	}
yy53:
	yyaccept = 0;
	yych = *(marker = ++cursor);
	switch (yych) {
		case 0x00: goto yy19;
		case 'u': goto yy69;
		default: goto yy18;
	}
yy54:
	yyaccept = 0;
	yych = *(marker = ++cursor);
	switch (yych) {
		case 0x00: goto yy19;
		case 'r': goto yy70;
		default: goto yy18;
	}
yy55:
	yyaccept = 0;
	yych = *(marker = ++cursor);
	switch (yych) {
		case 0x00: goto yy19;
		case 'i': goto yy72;
		default: goto yy18;
	}
yy56:
	yych = *++cursor;
	switch (yych) {
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
		case 'z': goto yy73;
		default: goto yy38;
	}
yy57:
	yyaccept = 0;
	yych = *(marker = ++cursor);
	switch (yych) {
		case 0x00: goto yy19;
		case 'l': goto yy75;
		default: goto yy18;
	}
yy58:
	yyaccept = 4;
	yych = *(marker = ++cursor);
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
		case ':':
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
		case 'z': goto yy18;
		default: goto yy59;
	}
yy59:
#line 181 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::IntType; }
#line 1058 "src/front/Lexer.cpp"
yy60:
	yyaccept = 5;
	yych = *(marker = ++cursor);
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
		case ':':
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
		case 'z': goto yy18;
		default: goto yy61;
	}
yy61:
#line 180 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::NilType; }
#line 1132 "src/front/Lexer.cpp"
yy62:
	yyaccept = 0;
	yych = *(marker = ++cursor);
	switch (yych) {
		case 0x00: goto yy19;
		case 'e': goto yy77;
		default: goto yy18;
	}
yy63:
	yyaccept = 0;
	yych = *(marker = ++cursor);
	switch (yych) {
		case 0x00: goto yy19;
		case 's': goto yy79;
		default: goto yy18;
	}
yy64:
	yyaccept = 6;
	yych = *(marker = ++cursor);
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
		case ':':
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
		case 'z': goto yy18;
		default: goto yy65;
	}
yy65:
#line 179 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::Nil; }
#line 1222 "src/front/Lexer.cpp"
yy66:
	yyaccept = 7;
	yych = *(marker = ++cursor);
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
		case ':':
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
		case 'z': goto yy18;
		default: goto yy67;
	}
yy67:
#line 185 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::Ptr; }
#line 1296 "src/front/Lexer.cpp"
yy68:
	yyaccept = 0;
	yych = *(marker = ++cursor);
	switch (yych) {
		case 0x00: goto yy19;
		case 'n': goto yy80;
		default: goto yy18;
	}
yy69:
	yyaccept = 0;
	yych = *(marker = ++cursor);
	switch (yych) {
		case 0x00: goto yy19;
		case 'e': goto yy82;
		default: goto yy18;
	}
yy70:
	yyaccept = 8;
	yych = *(marker = ++cursor);
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
		case ':':
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
		case 'z': goto yy18;
		default: goto yy71;
	}
yy71:
#line 188 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::Var; }
#line 1386 "src/front/Lexer.cpp"
yy72:
	yyaccept = 0;
	yych = *(marker = ++cursor);
	switch (yych) {
		case 0x00: goto yy19;
		case 'l': goto yy84;
		default: goto yy18;
	}
yy73:
	yyaccept = 9;
	yych = *(marker = ++cursor);
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
		case 'z': goto yy73;
		case ':': goto yy37;
		default: goto yy74;
	}
yy74:
#line 210 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::FullyQualifiedId; }
#line 1468 "src/front/Lexer.cpp"
yy75:
	yyaccept = 10;
	yych = *(marker = ++cursor);
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
		case ':':
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
		case 'z': goto yy18;
		default: goto yy76;
	}
yy76:
#line 184 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::BoolType; }
#line 1542 "src/front/Lexer.cpp"
yy77:
	yyaccept = 11;
	yych = *(marker = ++cursor);
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
		case ':':
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
		case 'z': goto yy18;
		default: goto yy78;
	}
yy78:
#line 191 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::Else; }
#line 1616 "src/front/Lexer.cpp"
yy79:
	yyaccept = 0;
	yych = *(marker = ++cursor);
	switch (yych) {
		case 0x00: goto yy19;
		case 'e': goto yy85;
		default: goto yy18;
	}
yy80:
	yyaccept = 12;
	yych = *(marker = ++cursor);
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
		case ':':
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
		case 'z': goto yy18;
		default: goto yy81;
	}
yy81:
#line 190 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::Then; }
#line 1698 "src/front/Lexer.cpp"
yy82:
	yyaccept = 13;
	yych = *(marker = ++cursor);
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
		case ':':
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
		case 'z': goto yy18;
		default: goto yy83;
	}
yy83:
#line 182 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::True; }
#line 1772 "src/front/Lexer.cpp"
yy84:
	yyaccept = 0;
	yych = *(marker = ++cursor);
	switch (yych) {
		case 0x00: goto yy19;
		case 'e': goto yy87;
		default: goto yy18;
	}
yy85:
	yyaccept = 14;
	yych = *(marker = ++cursor);
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
		case ':':
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
		case 'z': goto yy18;
		default: goto yy86;
	}
yy86:
#line 183 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::False; }
#line 1854 "src/front/Lexer.cpp"
yy87:
	yyaccept = 15;
	yych = *(marker = ++cursor);
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
		case ':':
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
		case 'z': goto yy18;
		default: goto yy88;
	}
yy88:
#line 192 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::While; }
#line 1928 "src/front/Lexer.cpp"
yy89:
#line 216 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::End; }
#line 1932 "src/front/Lexer.cpp"
}
#line 217 "src/front/Lexer.re"

  }
}
// NOLINTEND(cppcoreguidelines-avoid-goto)
} // namespace pink
