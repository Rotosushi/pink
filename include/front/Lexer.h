#pragma once
#include <string>

#include "aux/Location.h"
#include "front/Token.h"

namespace pink {
    class Lexer {
    private:
        Location     loc;
        std::string  buffer;
        std::string::iterator end;
        std::string::iterator cursor;
        std::string::iterator marker;
        std::string::iterator token;

        void UpdateLoc();
        // #!WARN! #NOTE: std::string::iterator it -must- be a valid iterator
        // into the string referenced by buf. 
        int  Distance(std::string& buf, std::string::iterator it);
        // returns and iterator to the string buf, at some offset from the
        // beginning.
        std::string::iterator SeekIterator(std::string& buf, int distance);

    public:
        Lexer();
        Lexer(std::string& buf);

        std::string& GetBuf();
        void SetBuf(std::string& buf);
        void AppendBuf(std::string& text);
        void Reset();
        bool EndOfInput();

        Token       yylex();
        std::string yytxt();
        Location    yyloc();
    };
}
