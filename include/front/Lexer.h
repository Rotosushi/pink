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

    public:
        Lexer();
        Lexer(std::string& buf);

        void SetBuf(std::string& buf);
        void Reset();

        Token       yylex();
        std::string yytxt();
        Location    yyloc();
    };
}
