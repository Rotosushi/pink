#include "aux/Error.h"   // pink::Error
#include "aux/Outcome.h" // pink::Outcome<>

#include "ast/Ast.h"     // pink::Ast
#include "type/Type.h"   // pink::Type

#include "front/Lexer.h" // pink::Lexer pink::Token

#include "ops/PrecedenceAndAssociativity.h"

namespace pink {
    class Environment;


    class Parser {
    private:
        Lexer lexer;
        Token tok;
        Location loc;
        std::string txt;

        // primes tok, loc, and txt, with their new values
        // from the next token in the buffer.
        void nexttok();

        Outcome<Ast*, Error> ParseTerm(Environment& env);
        Outcome<Ast*, Error> ParseAffix(Environment& env);
        Outcome<Ast*, Error> ParseInfix(Ast* right, Precedence precedence, Environment& env);
        Outcome<Ast*, Error> ParseBasic(Environment& env);
        Outcome<Type*, Error> ParseBasicType(Environment& env);
    public:
        Parser();
        ~Parser();

        Outcome<Ast*, Error> Parse(std::string str, Environment& env);
    };
}
