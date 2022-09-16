#pragma once
#include <iostream>

#include "aux/Error.h"   // pink::Error
#include "aux/Outcome.h" // pink::Outcome<>

#include "ast/Ast.h"     // pink::Ast
#include "type/Type.h"   // pink::Type

#include "front/Lexer.h" // pink::Lexer pink::Token

#include "aux/StringInterner.h"

#include "ops/PrecedenceAndAssociativity.h"

namespace pink {
    class Environment;


    class Parser {
    private:
        Lexer         lexer;
        Token         tok;
        Location      loc;
        std::string   txt;
        std::istream* input_stream;

        void yyfill();
        // primes tok, loc, and txt, with their new values
        // from the next token in the buffer.
        void nexttok();

        Outcome<std::unique_ptr<Ast>, Error> ParseTerm(const Environment& env);
        Outcome<std::unique_ptr<Ast>, Error> ParseAffix(const Environment& env);
        Outcome<std::unique_ptr<Ast>, Error> ParseComposite(const Environment& env);
        Outcome<std::unique_ptr<Ast>, Error> ParseDot(const Environment& env);
        Outcome<std::unique_ptr<Ast>, Error> ParseInfix(std::unique_ptr<Ast> left, Precedence precedence, const Environment& env);
        Outcome<std::unique_ptr<Ast>, Error> ParseBasic(const Environment& env);
        Outcome<std::unique_ptr<Ast>, Error> ParseBlock(const Environment& env);
        Outcome<std::unique_ptr<Ast>, Error> ParseConditional(const Environment& env);
        Outcome<std::unique_ptr<Ast>, Error> ParseWhile(const Environment& env);
        Outcome<std::unique_ptr<Ast>, Error> ParseFunction(const Environment& env);
        Outcome<std::pair<InternedString, Type*>, Error> ParseArgument(const Environment& env);
        Outcome<Type*, Error> ParseType(const Environment& env);
        Outcome<Type*, Error> ParseBasicType(const Environment& env);
    public:
        Parser(); // by default the parser reads input from std::cin
        Parser(std::istream* input_stream);
        ~Parser();
       
        bool EndOfInput(); 
        const std::string& GetBuf();
        std::istream* GetIStream();
        void SetIStream(std::istream* input_stream);
        // acts exactly as std::getline, filling the buffer passed
        // from the input stream until it encounters the newline char '\n'
        // except that unlike std::getline it appends the '\n' 
        // to the buffer. This is to preserve the '\n' for the 
        // lexer in order for the lexer to correctly count it's
        // position within the source file.
        void Getline(std::string& str);
        
        Outcome<std::unique_ptr<Ast>, Error> Parse(const Environment& env);
    };
}
