/* Generated by re2c 2.2 on Tue Jan 11 17:22:06 2022 */
#line 1 "src/front/Lexer.re"

#include "front/Lexer.hpp"

namespace pink {
    Lexer::Lexer()
        : loc(), buffer()
    {
        end = cursor = marker = token = buffer.end();
    }

    Lexer::Lexer(std::string& buf)
        : loc(), buffer(buf)
    {
        end = buffer.end();
        cursor = marker = token = buffer.begin();
    }

    void Lexer::SetBuf(std::string& buf)
    {
        buffer = buf;
        end = buffer.end();
        cursor = marker = token = buffer.begin();
    }

    void Lexer::Reset()
    {
        buffer.clear();
        end = cursor = marker = token = buffer.end();
    }

    /*
        This routine works great for single line
        inputs, but should be tested against multi-line
        inputs for a more rigourous compiler. #TODO
    */
    void Lexer::UpdateLoc()
    {
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
        size_t length = cursor - token;

        loc.firstLine   = loc.lastLine;
        loc.firstColumn = loc.lastColumn;

        for (size_t i = 0; i < length; i++)
        {
            if (token[i] == '\n')
            {
                loc.lastLine   += 1;
                loc.lastColumn += 1;
            }
            else
            {
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
    std::string Lexer::yytxt()
    {
        return std::string(token, cursor);
    }

    Location Lexer::yyloc()
    {
        return loc;
    }


    /*
        These are the definitions of the parsing
        primitives that re2c uses, such that we
        can interoperate between c++ and re2c
    */
    #line 115 "src/front/Lexer.re"


    Token Lexer::yylex()
    {
        while (1)
        {
            token = cursor;
            
#line 109 "src/front/Lexer.cpp"
{
	char yych;
	yych = *cursor;
	switch (yych) {
	case '\t':
	case '\n':
	case ' ':	goto yy4;
	case '!':
	case '$':
	case '%':
	case '&':
	case '*':
	case '+':
	case '-':
	case '<':
	case '>':
	case '@':
	case '\\':
	case '^':
	case '|':
	case '~':	goto yy6;
	case '(':	goto yy9;
	case ')':	goto yy11;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy13;
	case ':':	goto yy16;
	case '=':	goto yy18;
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
	case 'd':
	case 'e':
	case 'g':
	case 'h':
	case 'i':
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
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy20;
	case 'B':	goto yy21;
	case 'I':	goto yy22;
	case 'N':	goto yy23;
	case 'f':	goto yy24;
	case 'n':	goto yy25;
	case 't':	goto yy26;
	default:
		if (end <= cursor) goto yy52;
		goto yy2;
	}
yy2:
	++cursor;
yy3:
#line 123 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::Error; }
#line 207 "src/front/Lexer.cpp"
yy4:
	++cursor;
#line 125 "src/front/Lexer.re"
	{ UpdateLoc(); continue; }
#line 212 "src/front/Lexer.cpp"
yy6:
	yych = *++cursor;
	switch (yych) {
	case '!':
	case '$':
	case '%':
	case '&':
	case '*':
	case '+':
	case '-':
	case '<':
	case '=':
	case '>':
	case '@':
	case '\\':
	case '^':
	case '|':
	case '~':	goto yy6;
	default:	goto yy8;
	}
yy8:
#line 141 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::Op; }
#line 236 "src/front/Lexer.cpp"
yy9:
	++cursor;
#line 137 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::LParen; }
#line 241 "src/front/Lexer.cpp"
yy11:
	++cursor;
#line 138 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::RParen; }
#line 246 "src/front/Lexer.cpp"
yy13:
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
	case '9':	goto yy13;
	default:	goto yy15;
	}
yy15:
#line 129 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::Int; }
#line 265 "src/front/Lexer.cpp"
yy16:
	++cursor;
#line 135 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::Colon; }
#line 270 "src/front/Lexer.cpp"
yy18:
	yych = *++cursor;
	switch (yych) {
	case '!':
	case '$':
	case '%':
	case '&':
	case '*':
	case '+':
	case '-':
	case '<':
	case '=':
	case '>':
	case '@':
	case '\\':
	case '^':
	case '|':
	case '~':	goto yy6;
	default:	goto yy19;
	}
yy19:
#line 136 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::Equals; }
#line 294 "src/front/Lexer.cpp"
yy20:
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
	case 'z':	goto yy27;
	default:	goto yy3;
	}
yy21:
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
	case 'z':	goto yy27;
	case 'o':	goto yy30;
	default:	goto yy3;
	}
yy22:
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
	case 'z':	goto yy27;
	case 'n':	goto yy31;
	default:	goto yy3;
	}
yy23:
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
	case 'z':	goto yy27;
	case 'i':	goto yy32;
	default:	goto yy3;
	}
yy24:
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
	case 'z':	goto yy27;
	case 'a':	goto yy33;
	default:	goto yy3;
	}
yy25:
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
	case 'z':	goto yy27;
	case 'i':	goto yy34;
	default:	goto yy3;
	}
yy26:
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
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy27;
	case 'r':	goto yy35;
	default:	goto yy3;
	}
yy27:
	yych = *++cursor;
yy28:
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
	case 'z':	goto yy27;
	default:	goto yy29;
	}
yy29:
#line 140 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::Id; }
#line 843 "src/front/Lexer.cpp"
yy30:
	yych = *++cursor;
	switch (yych) {
	case 0x00:	goto yy29;
	case 'o':	goto yy36;
	default:	goto yy28;
	}
yy31:
	yych = *++cursor;
	switch (yych) {
	case 0x00:	goto yy29;
	case 't':	goto yy37;
	default:	goto yy28;
	}
yy32:
	yych = *++cursor;
	switch (yych) {
	case 0x00:	goto yy29;
	case 'l':	goto yy39;
	default:	goto yy28;
	}
yy33:
	yych = *++cursor;
	switch (yych) {
	case 0x00:	goto yy29;
	case 'l':	goto yy41;
	default:	goto yy28;
	}
yy34:
	yych = *++cursor;
	switch (yych) {
	case 0x00:	goto yy29;
	case 'l':	goto yy42;
	default:	goto yy28;
	}
yy35:
	yych = *++cursor;
	switch (yych) {
	case 0x00:	goto yy29;
	case 'u':	goto yy44;
	default:	goto yy28;
	}
yy36:
	yych = *++cursor;
	switch (yych) {
	case 0x00:	goto yy29;
	case 'l':	goto yy45;
	default:	goto yy28;
	}
yy37:
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
	case 'z':	goto yy27;
	default:	goto yy38;
	}
yy38:
#line 130 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::IntType; }
#line 964 "src/front/Lexer.cpp"
yy39:
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
	case 'z':	goto yy27;
	default:	goto yy40;
	}
yy40:
#line 128 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::NilType; }
#line 1036 "src/front/Lexer.cpp"
yy41:
	yych = *++cursor;
	switch (yych) {
	case 0x00:	goto yy29;
	case 's':	goto yy47;
	default:	goto yy28;
	}
yy42:
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
	case 'z':	goto yy27;
	default:	goto yy43;
	}
yy43:
#line 127 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::Nil; }
#line 1115 "src/front/Lexer.cpp"
yy44:
	yych = *++cursor;
	switch (yych) {
	case 0x00:	goto yy29;
	case 'e':	goto yy48;
	default:	goto yy28;
	}
yy45:
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
	case 'z':	goto yy27;
	default:	goto yy46;
	}
yy46:
#line 133 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::BoolType; }
#line 1194 "src/front/Lexer.cpp"
yy47:
	yych = *++cursor;
	switch (yych) {
	case 0x00:	goto yy29;
	case 'e':	goto yy50;
	default:	goto yy28;
	}
yy48:
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
	case 'z':	goto yy27;
	default:	goto yy49;
	}
yy49:
#line 131 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::True; }
#line 1273 "src/front/Lexer.cpp"
yy50:
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
	case 'z':	goto yy27;
	default:	goto yy51;
	}
yy51:
#line 132 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::False; }
#line 1345 "src/front/Lexer.cpp"
yy52:
#line 124 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::End; }
#line 1349 "src/front/Lexer.cpp"
}
#line 142 "src/front/Lexer.re"

        }
    }
}
