#include "Test.h"
#include "front/TestToken.h"
#include "front/Token.h"


bool Testlet(std::string txt, pink::Token tok)
{
    bool result = true;

    if (txt != pink::TokenToString(tok))
        result = false;

    return result;
}


bool TestToken(std::ostream& out)
{
    bool result = true;
    out << "\n-----------------------\n";
    out << "Testing pink::Token: \n";

    result &= Test(out, "Token::Error", Testlet("Token::Error", pink::Token::Error));
    result &= Test(out, "Token::End", Testlet("Token::End", pink::Token::End));
    result &= Test(out, "Token::Id", Testlet("Token::Id", pink::Token::Id));
    result &= Test(out, "Token::Op", Testlet("Token::Op", pink::Token::Op));
    result &= Test(out, "Token::Colon", Testlet("Token::Colon", pink::Token::Colon));
    result &= Test(out, "Token::Equals", Testlet("Token::Equals", pink::Token::Equals));
    result &= Test(out, "Token::LParen", Testlet("Token::LParen", pink::Token::LParen));
    result &= Test(out, "Token::RParen", Testlet("Token::RParen", pink::Token::RParen));
    result &= Test(out, "Token::Nil", Testlet("Token::Nil", pink::Token::Nil));
    result &= Test(out, "Token::NilType", Testlet("Token::NilType", pink::Token::NilType));
    result &= Test(out, "Token::Int", Testlet("Token::Int", pink::Token::Int));
    result &= Test(out, "Token::IntType", Testlet("Token::IntType", pink::Token::IntType));
    result &= Test(out, "Token::True", Testlet("Token::True", pink::Token::True));
    result &= Test(out, "Token::False", Testlet("Token::False", pink::Token::False));
    result &= Test(out, "Token::BoolType", Testlet("Token::BoolType", pink::Token::BoolType));

    result &= Test(out, "pink::Token", result);
    out << "\n-----------------------\n";
    return result;
}
