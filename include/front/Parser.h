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

        Outcome<std::unique_ptr<Ast>, Error> ParseTerm(std::shared_ptr<Environment> env);
        Outcome<std::unique_ptr<Ast>, Error> ParseAffix(std::shared_ptr<Environment> env);
        Outcome<std::unique_ptr<Ast>, Error> ParseComposite(std::shared_ptr<Environment> env);
        Outcome<std::unique_ptr<Ast>, Error> ParseInfix(std::unique_ptr<Ast> left, Precedence precedence, std::shared_ptr<Environment>);
        Outcome<std::unique_ptr<Ast>, Error> ParseBasic(std::shared_ptr<Environment> env);
        Outcome<std::unique_ptr<Ast>, Error> ParseBlock(std::shared_ptr<Environment> env);
        Outcome<std::unique_ptr<Ast>, Error> ParseFunction(std::shared_ptr<Environment> env);
        Outcome<std::pair<InternedString, Type*>, Error> ParseArgument(std::shared_ptr<Environment> env);
        Outcome<Type*, Error> ParseBasicType(std::shared_ptr<Environment> env);
    public:
        Parser(std::istream* input_stream);
        ~Parser();
       
        bool EndOfInput(); 
        const std::string& GetBuf();
        std::istream* GetIStream();
        void SetIStream(std::istream* i);
        
        Outcome<std::unique_ptr<Ast>, Error> Parse(std::shared_ptr<Environment> env);
    };
}
