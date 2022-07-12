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
        Lexer lexer;
        Token tok;
        Location loc;
        std::string txt;
        std::istream* input_stream;

        std::string& GetBuf();
        void yyfill();
        // primes tok, loc, and txt, with their new values
        // from the next token in the buffer.
        void nexttok();

        Outcome<std::unique_ptr<Ast>, Error> ParseAffix(std::shared_ptr<Environment> env);
        Outcome<std::unique_ptr<Ast>, Error> ParseInfix(std::unique_ptr<Ast> right, Precedence precedence, std::shared_ptr<Environment> env);
        Outcome<std::unique_ptr<Ast>, Error> ParseBasic(std::shared_ptr<Environment> env);
        Outcome<std::unique_ptr<Ast>, Error> ParseBlock(std::shared_ptr<Environment> env);
        Outcome<std::unique_ptr<Ast>, Error> ParseFunction(std::shared_ptr<Environment> env);
        Outcome<std::pair<InternedString, Type*>, Error> ParseArgument(std::shared_ptr<Environment> env);
        Outcome<Type*, Error> ParseBasicType(std::shared_ptr<Environment> env);
    public:
        Parser();
        ~Parser();
        
        Outcome<std::unique_ptr<Ast>, Error> Parse(std::string str, std::shared_ptr<Environment> env, std::istream* input_stream);
        Outcome<std::unique_ptr<Ast>, Error> Parse(std::string str, std::shared_ptr<Environment> env);
    };
}
