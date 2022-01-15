/* Generated by re2c 2.2 on Fri Jan 14 22:37:26 2022 */
#line 1 "src/front/Lexer.re"

#include "front/Lexer.h"

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
         
            
#line 110 "src/front/Lexer.cpp"
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
	case 'B':	goto yy23;
	case 'I':	goto yy24;
	case 'N':	goto yy25;
	case 'f':	goto yy26;
	case 'n':	goto yy27;
	case 't':	goto yy28;
	default:
		if (end <= cursor) goto yy53;
		goto yy2;
	}
yy2:
	++cursor;
#line 142 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::Error; }
#line 207 "src/front/Lexer.cpp"
yy4:
	++cursor;
#line 141 "src/front/Lexer.re"
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
#line 138 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::Op; }
#line 236 "src/front/Lexer.cpp"
yy9:
	++cursor;
#line 134 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::LParen; }
#line 241 "src/front/Lexer.cpp"
yy11:
	++cursor;
#line 135 "src/front/Lexer.re"
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
#line 139 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::Int; }
#line 265 "src/front/Lexer.cpp"
yy16:
	yych = *++cursor;
	switch (yych) {
	case '=':	goto yy29;
	default:	goto yy17;
	}
yy17:
#line 131 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::Colon; }
#line 275 "src/front/Lexer.cpp"
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
#line 132 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::Equals; }
#line 299 "src/front/Lexer.cpp"
yy20:
	yych = *++cursor;
yy21:
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
	case 'z':	goto yy20;
	default:	goto yy22;
	}
yy22:
#line 137 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::Id; }
#line 372 "src/front/Lexer.cpp"
yy23:
	yych = *++cursor;
	switch (yych) {
	case 0x00:	goto yy22;
	case 'o':	goto yy31;
	default:	goto yy21;
	}
yy24:
	yych = *++cursor;
	switch (yych) {
	case 0x00:	goto yy22;
	case 'n':	goto yy32;
	default:	goto yy21;
	}
yy25:
	yych = *++cursor;
	switch (yych) {
	case 0x00:	goto yy22;
	case 'i':	goto yy33;
	default:	goto yy21;
	}
yy26:
	yych = *++cursor;
	switch (yych) {
	case 0x00:	goto yy22;
	case 'a':	goto yy34;
	default:	goto yy21;
	}
yy27:
	yych = *++cursor;
	switch (yych) {
	case 0x00:	goto yy22;
	case 'i':	goto yy35;
	default:	goto yy21;
	}
yy28:
	yych = *++cursor;
	switch (yych) {
	case 0x00:	goto yy22;
	case 'r':	goto yy36;
	default:	goto yy21;
	}
yy29:
	++cursor;
#line 133 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::ColonEq; }
#line 419 "src/front/Lexer.cpp"
yy31:
	yych = *++cursor;
	switch (yych) {
	case 0x00:	goto yy22;
	case 'o':	goto yy37;
	default:	goto yy21;
	}
yy32:
	yych = *++cursor;
	switch (yych) {
	case 0x00:	goto yy22;
	case 't':	goto yy38;
	default:	goto yy21;
	}
yy33:
	yych = *++cursor;
	switch (yych) {
	case 0x00:	goto yy22;
	case 'l':	goto yy40;
	default:	goto yy21;
	}
yy34:
	yych = *++cursor;
	switch (yych) {
	case 0x00:	goto yy22;
	case 'l':	goto yy42;
	default:	goto yy21;
	}
yy35:
	yych = *++cursor;
	switch (yych) {
	case 0x00:	goto yy22;
	case 'l':	goto yy43;
	default:	goto yy21;
	}
yy36:
	yych = *++cursor;
	switch (yych) {
	case 0x00:	goto yy22;
	case 'u':	goto yy45;
	default:	goto yy21;
	}
yy37:
	yych = *++cursor;
	switch (yych) {
	case 0x00:	goto yy22;
	case 'l':	goto yy46;
	default:	goto yy21;
	}
yy38:
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
	case 'z':	goto yy20;
	default:	goto yy39;
	}
yy39:
#line 126 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::IntType; }
#line 540 "src/front/Lexer.cpp"
yy40:
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
	case 'z':	goto yy20;
	default:	goto yy41;
	}
yy41:
#line 125 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::NilType; }
#line 612 "src/front/Lexer.cpp"
yy42:
	yych = *++cursor;
	switch (yych) {
	case 0x00:	goto yy22;
	case 's':	goto yy48;
	default:	goto yy21;
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
	case 'z':	goto yy20;
	default:	goto yy44;
	}
yy44:
#line 124 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::Nil; }
#line 691 "src/front/Lexer.cpp"
yy45:
	yych = *++cursor;
	switch (yych) {
	case 0x00:	goto yy22;
	case 'e':	goto yy49;
	default:	goto yy21;
	}
yy46:
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
	case 'z':	goto yy20;
	default:	goto yy47;
	}
yy47:
#line 129 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::BoolType; }
#line 770 "src/front/Lexer.cpp"
yy48:
	yych = *++cursor;
	switch (yych) {
	case 0x00:	goto yy22;
	case 'e':	goto yy51;
	default:	goto yy21;
	}
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
	case 'z':	goto yy20;
	default:	goto yy50;
	}
yy50:
#line 127 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::True; }
#line 849 "src/front/Lexer.cpp"
yy51:
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
	case 'z':	goto yy20;
	default:	goto yy52;
	}
yy52:
#line 128 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::False; }
#line 921 "src/front/Lexer.cpp"
yy53:
#line 143 "src/front/Lexer.re"
	{ UpdateLoc(); return Token::End; }
#line 925 "src/front/Lexer.cpp"
}
#line 144 "src/front/Lexer.re"

        }
    }
}
