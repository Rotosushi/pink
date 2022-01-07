#include "front/TestLexer.hpp"
#include "front/Lexer.hpp"


bool TestLexer(std::ostream& out)
{
    bool result = true;
    out << "\n-----------------------\n";
    out << "Testing Pink::Lexer: \n";

    pink::Lexer lex;

    std::string id("symbol");
    pink::Location id_loc(0, 0, 0, 6);

    lex.SetBuf(id);

    pink::Token    tok = lex.yylex();
    pink::Location loc = lex.yyloc();
    std::string    txt = lex.yytxt();

    if (tok == pink::Token::Id)
    {
        out << "\tTest: Lexer::yylex(), Token::Id: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Lexer::yylex(), Token::Id: Failed\n";
    }

    if (loc == id_loc)
    {
        out << "\tTest: Lexer::yyloc(), Token::Id: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest:: Lexer::yyloc(), Token::Id: Passed\n";
    }

    if (txt == id)
    {
        out << "\tTest: Lexer::yytxt(), Token::Id: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Lexer::yytxt(), Token::Id: Failed\n";
    }


    if (result)
        out << "Test: pink::Lexer: Passed\n";
    else
        out << "Test: pink::Lexer: Failed\n";

    out << "\n-----------------------\n";
    return result;
}
