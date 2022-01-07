
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

        id = [a-zA-Z_][a-zA-Z0-9_]+;
        op = [+\-*\\%<=>&|\^!~@$]+;
        int = [0-9]+;
    */

    Token Lexer::yylex()
    {
        while (1)
        {
            token = cursor;
            /*!re2c
                *   { UpdateLoc(); return Token::Error; } // Unknown Token
                $   { UpdateLoc(); return Token::End; } // End of Input
                [ \t\n] { UpdateLoc(); continue; } // Whitespace

                "nil"   { UpdateLoc(); return Token::Nil; }
                "Nil"   { UpdateLoc(); return Token::NilType; }
                int     { UpdateLoc(); return Token::Int; }
                "Int"   { UpdateLoc(); return Token::IntType; }
                "true"  { UpdateLoc(); return Token::True; }
                "false" { UpdateLoc(); return Token::False; }
                "Bool"  { UpdateLoc(); return Token::BoolType; }

                ":"     { UpdateLoc(); return Token::Colon; }
                "="     { UpdateLoc(); return Token::Assign; }
                "("     { UpdateLoc(); return Token::LParen; }
                ")"     { UpdateLoc(); return Token::RParen; }

                id      { UpdateLoc(); return Token::Id; }
                op      { UpdateLoc(); return Token::Op; }
            */
        }
    }
}
