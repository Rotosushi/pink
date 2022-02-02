#include "Test.h"
#include "front/TestLexer.h"
#include "front/Lexer.h"

/*
    Test that the lexer validly lexes the provided
    token, that the provided textual location matches
    the lexed one, and that the lexed string matches the
    input.
*/
bool TestToken(std::ostream& out, pink::Lexer lex, pink::Token t, pink::Location l, std::string b)
{
    bool result = true;

    std::string tokstr = TokenToString(t);

    lex.SetBuf(b);

    pink::Token    tok = lex.yylex();
    pink::Location loc = lex.yyloc();
    std::string    txt = lex.yytxt();

    result &= Test(out, std::string("Lexer::yylex(), " + tokstr), tok == t);
    result &= Test(out, std::string("Lexer::yyloc(), " + tokstr), loc == l);
    result &= Test(out, std::string("Lexer::yytxt(), " + tokstr), txt == b);

    return result;
}


bool TestLexer(std::ostream& out)
{
    bool result = true;
    out << "\n-----------------------\n";
    out << "Testing Pink::Lexer: \n";

    pink::Lexer lex;

    std::string str("symbol");
    pink::Location loc(0, 0, 0, 6);

    result &= TestToken(out, lex, pink::Token::Id, loc, str);

    str = "+";
    loc = {0, 0, 0, 1};

    result &= TestToken(out, lex, pink::Token::Op, loc, str);

	str = ",";
	
	result &= TestToken(out, lex, pink::Token::Comma, loc, str);
	
	str = ";";
	
	result &= TestToken(out, lex, pink::Token::Semicolon, loc, str);

    str = ":";

    result &= TestToken(out, lex, pink::Token::Colon, loc, str);
    
    str = "(";
    
    result &= TestToken(out, lex, pink::Token::LParen, loc, str);
    
    str = ")";
    
    result &= TestToken(out, lex, pink::Token::RParen, loc, str);
    
    str = "{";
    
    result &= TestToken(out, lex, pink::Token::LBrace, loc, str);
    
    str = "}";
    
    result &= TestToken(out, lex, pink::Token::RBrace, loc, str);

    str = "=";

    result &= TestToken(out, lex, pink::Token::Equals, loc, str);

    str = "(";

    result &= TestToken(out, lex, pink::Token::LParen, loc, str);

    str = ")";

    result &= TestToken(out, lex, pink::Token::RParen, loc, str);

	str = ":=";
    loc = {0, 0, 0, 2};
    
    result &= TestToken(out, lex, pink::Token::ColonEq, loc, str);
	

    // each time the size of the text being lexed changes, so
    // will the resulting location emitted from the lexer,
    // so we must modify the test lexer to match.
    str = "nil";
    loc = {0, 0, 0, 3};

    result &= TestToken(out, lex, pink::Token::Nil, loc, str);

    str = "Nil";

    result &= TestToken(out, lex, pink::Token::NilType, loc, str);

    str = "108"; // 3 digits means it will have the same location as nil.

    result &= TestToken(out, lex, pink::Token::Int, loc, str);

    str = "Int";

    result &= TestToken(out, lex, pink::Token::IntType, loc, str);

    str = "true";
    loc = {0, 0, 0, 4};

    result &= TestToken(out, lex, pink::Token::True, loc, str);

    str = "false";
    loc = {0, 0, 0, 5};

    result &= TestToken(out, lex, pink::Token::False, loc, str);

    str = "Bool";
    loc = {0, 0, 0, 4};

    result &= TestToken(out, lex, pink::Token::BoolType, loc, str);
    
    str = "fn";
    loc = {0, 0, 0, 2};
    
    result &= TestToken(out, lex, pink::Token::Fn, loc, str);


    result &= Test(out, "pink::Lexer", result);

    out << "\n-----------------------\n";
    return result;
}
