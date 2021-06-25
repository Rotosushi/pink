#include <string>
#include <memory>

#include "Parser.hpp"

#include "Lexer.hpp"

namespace pink {

Lexer::Lexer(const Environment& env)
  : env(env), loc(), buffer(),
{
  end = buffer.end();
  cursor = marker = token = buffer.begin();
}

Lexer::Lexer(const std::string& data, const Environment& env)
  : env(env), loc(), buffer(data)
{
  end = buffer.end();
  cursor = marker = token = buffer.begin();
}

void Lexer::SetBuffer(const std::string& data)
{
  buffer = data;
  end = buffer.end();
  cursor = marker = token = buffer.begin();
}

void Lexer::Reset()
{
  buffer.clear();
  end = buffer.end();
  cursor = marker = token = buffer.begin();
}

void Lexer::update_loc()
{
  std::size_t length = cursor - token;
  loc.first_line     = loc.last_line;
  loc.first_column   = loc.last_column;

  for (std::size_t i = 0; i < length; i++)
  {
    if (token[i] == '\n')
    {
      loc.last_line  += 1;
      loc.last_column = 1;
    }
    else
    {
      loc.last_column += 1;
    }
  }
}

std::string Lexer::yytxt()
{
/*
        this was once
        strndup(token, cursor - token);
        where token and cursor have type char*
        (and are pointing into the buffer of characters
        to lex.)

        so here we emulate said behavior by copying
        the series of characters from token until cursor
        we utilize the string constructor defined
        with two string iterators to achieve the same
        result.
*/
  return std::string(token, cursor);
}

Location Lexer::yyloc()
{
  return loc;
}


// these definitions are for re2c
// first is some definitions of basic
// regular expressions defining primitives
// in the language.
/*!re2c
    identifier = [a-zA-Z_][a-zA-Z0-9_]+;
    integer    = [0-9]+;
    operator   = [+\-*/%<=>&@!|$\^]+;

*/

// then we have these macros, which define
// the operating primitives of the re2c code itself.
#define YYPEEK()      (*cursor)
#define YYSKIP()      (++cursor)
#define YYBACKUP()    (marker = cursor)
#define YYRESTORE()   (cursor = marker)
#define YYLESSTHAN(n) (n > (end - cursor))


Token Lexer::yylex()
{
    while(1)
    {
        token = cursor;
        /*!re2c
            re2c:define:YYCTYPE = char;
            re2c:yyfill:enable  = 0;
            re2c:eof            = 0;

            *          { update_loc(); return Token::Error;        } // unknown
            $          { update_loc(); return Token::End;          } // end of input
            [ \t\n]    { update_loc(); continue;                   }
            "nil"      { update_loc(); return Token::Nil;          }
            "Nil"      { update_loc(); return Token::Type_Nil;     }
            "Int"      { update_loc(); return Token::Type_Integer; }
            "true"     { update_loc(); return Token::True;         }
            "false"    { update_loc(); return Token::False;        }
            "Bool"     { update_loc(); return Token::Type_Boolean; }
            "if"       { update_loc(); return Token::If;           }
            "then"     { update_loc(); return Token::Then;         }
            "else"     { update_loc(); return Token::Else;         }
            "while"    { update_loc(); return Token::While;        }
            "do"       { update_loc(); return Token::Do;           }
            "\\"       { update_loc(); return Token::Backslash;    }
            ","        { update_loc(); return Token::Comma;        }
            ":"        { update_loc(); return Token::Colon;        }
            "=>"       { update_loc(); return Token::Body;         }
            "<-"       { update_loc(); return Token::Assign;       }
            "("        { update_loc(); return Token::Lparen;       }
            ")"        { update_loc(); return Token::Rparen;       }
            "{"        { update_loc(); return Token::Lbrace;       }
            "}"        { update_loc(); return Token::Rbrace;       }
            "["        { update_loc(); return Token::Lbracket;     }
            "]"        { update_loc(); return Token::Rbracket;     }
            identifier { update_loc(); return Token::Identifier;   }
            integer    { update_loc(); return Token::Integer;      }
            operator   { update_loc(); return Token::Operator;     }
        */
    }
}


}










// --
