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

        std::string& GetBuf();
        void SetBuf(std::string& buf);
        void AppendBuf(const char* text);
        void AppendBuf(std::string& text);
        void Reset();
        bool EndOfInput();

        Token       yylex();
        std::string yytxt();
        Location    yyloc();
    };
}
