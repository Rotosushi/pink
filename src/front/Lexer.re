
#include "front/Lexer.h"

namespace pink {
    Lexer::Lexer()
        : loc(), buffer()
    {
        end = cursor = marker = token = buffer.end();
    }

    Lexer::Lexer(std::string& buf)
        : loc(), buffer(buf)
    {
        end = buffer.end();
        cursor = marker = token = buffer.begin();
    }

    void Lexer::SetBuf(std::string& buf)
    {
        buffer = buf;
        end = buffer.end();
        cursor = marker = token = buffer.begin();
    }

    void Lexer::AppendBuf(std::string& buf)
    {
      // this procedure modifies the buffer we are currently parsing 
      // by appending the text passed onto it.
      // normally this is inadviseable, because if the string being 
      // appened onto needs to reallocate its internal buffer to
      // accomidate the new characters then any iterators into 
      // said buffer will be invalidated, and we will not keep our 
      // location within the buffer. this is a problem, because we 
      // only want to add new text to lex, we don't want to destroy
      // our position within the array. so we can keep lexing where 
      // we left off. 
      // to solve this problem we are going to take advantage of the 
      // fact that when we perform the append operation, the original 
      // text, and position of said text within the new buffer will 
      // be identical to what it was before the append operation.
      // this is because the append operation does not modify the 
      // existing text, or its relative position within the buffer,
      // it only adds new text after the end.
      // thus if we store the distance that each iterator is into the 
      // current buffer, after the append, we can 'restore' the position 
      // of the iterators within the new buffer containing the 
      // old text and the appended new text.
      
      // save the relative offsets of each iterator.
      int cursor_dist = Distance(buffer, cursor);
      int marker_dist = Distance(buffer, marker);
      int token_dist  = Distance(buffer, token);
      
      buffer += buf; // perform the append operation, which may invalidate the iterators
      
      // construct new valid iterators, at their old relative offsets within the new string.
      end    = buffer.end();
      cursor = SeekIterator(buffer, cursor_dist);
      marker = SeekIterator(buffer, marker_dist);
      token  = SeekIterator(buffer, token_dist);
    }

    void Lexer::Reset()
    {
        buffer.clear();
        end = cursor = marker = token = buffer.end();
    }
    
    bool Lexer::EndOfInput()
    {
      return (end - cursor) == 0;
    }

    /*
        This routine works great for single line
        inputs, but should be tested against multi-line
        inputs for a more rigourous compiler. #TODO
    */
    void Lexer::UpdateLoc()
    {
    /*
        since token points to the beginning of the
        current token being lexed, and cursor points
        to the current position of the lexer,
        after a token has been lexed from the buffer,
        it sits between [cursor, token], and the
        string iterator operator -() lets us compute
        that distance in chars directly.

        theoretically we could swap string for wstring
        to support unicode. I think this would still
        work, but i'm not 100% sure about that.
    */
        size_t length = cursor - token;

        loc.firstLine   = loc.lastLine;
        loc.firstColumn = loc.lastColumn;

        for (size_t i = 0; i < length; i++)
        {
            if (token[i] == '\n')
            {
                loc.lastLine   += 1;
                loc.lastColumn += 1;
            }
            else
            {
                loc.lastColumn += 1;
            }
        }
    }

    int Lexer::Distance(std::string& buf, std::string::iterator it)
    {
      // the iterator points to somewhere in the array of characters,
      // the beginning of the buffer is somewhere in memory. since the 
      // data structure is a string, we know that the elements of memory 
      // between the beginning and it are also members of this string.
      // since the iterators are also just pointers, and pointers are 
      // just integers, we can subtract the farther point, it, from the 
      // closer point, buf.begin(), this results in the distance between the
      // two iterators. that is the number of characters within the string 
      // between the beginning and the iterator it.
      return it - buf.begin();
    }

    std::string::iterator Lexer::SeekIterator(std::string& buf, int distance)
    {
      // return an iterator into the string some distance into the string.
      return buf.begin() + distance;
    }


    /*
        token points to the beginning of the
        current token being lexed, and cursor points
        to the current position of the lexer,
        so the last token that was lexed is sitting
        between those two positions.

        Therefore we can use the string constructor
        taking a two iterators to construct the
        string from the characters from between that range.
    */
    std::string Lexer::yytxt()
    {
        return std::string(token, cursor);
    }

    Location Lexer::yyloc()
    {
        return loc;
    }


    /*
        These are the definitions of the parsing
        primitives that re2c uses, such that we
        can interoperate between c++ and re2c
        
        # TODO: i think this regex will allow for identifiers 
        	like: this-is-an-ident, follow-with-hyphen
        	but dissallow idents like:
        		-unop-application-not-an-ident,
        		binop-application-not-an-ident- more-text
        		 
        id=[a-zA-Z_]([-]?[a-zA-Z0-9_])*;
    */
    /*!re2c
        re2c:api:style = free-form;
        re2c:yyfill:enable = 0;
        re2c:eof = 0;

        re2c:define:YYCTYPE  = "char";
        re2c:define:YYCURSOR = "cursor";
        re2c:define:YYMARKER = "marker";
        re2c:define:YYLIMIT = "end";
        re2c:define:YYPEEK   = "(*cursor);";
        re2c:define:YYSKIP   = "(cursor++);";
        re2c:define:YYBACKUP = "(marker = cursor);";
        re2c:define:YYRESTORE = "(cursor = marker);";
        re2c:define:YYLESSTHAN = "(end > (end - cursor));";

        id=[a-zA-Z_][a-zA-Z0-9_]*;
        op=[+\-*\\%<=>&|\^!~@$]+;
        int=[0-9]+;
    */

	// NOLINTBEGIN(cppcoreguidelines-avoid-goto)
	// re2c uses gotos to implement the lexer, as all of the 
	// gotos are from generated code, we are trusting re2c to 
	// use gotos in a safe and sane way here.
    Token Lexer::yylex()
    {
        while (1)
        {
            token = cursor;
         
            /*!re2c               
                "nil"   { UpdateLoc(); return Token::Nil; }
                "Nil"   { UpdateLoc(); return Token::NilType; }
                "Int"   { UpdateLoc(); return Token::IntType; }
                "true"  { UpdateLoc(); return Token::True; }
                "false" { UpdateLoc(); return Token::False; }
                "Bool"  { UpdateLoc(); return Token::BoolType; }
                
                "fn"	{ UpdateLoc(); return Token::Fn; }

				","		{ UpdateLoc(); return Token::Comma; }
				";"		{ UpdateLoc(); return Token::Semicolon; }
                ":"     { UpdateLoc(); return Token::Colon; }
                "="     { UpdateLoc(); return Token::Equals; }
                ":="    { UpdateLoc(); return Token::ColonEq; }
                "("     { UpdateLoc(); return Token::LParen; }
                ")"     { UpdateLoc(); return Token::RParen; }
                "{"		{ UpdateLoc(); return Token::LBrace; }
                "}"		{ UpdateLoc(); return Token::RBrace; }
                
                id      { UpdateLoc(); return Token::Id; }
                op      { UpdateLoc(); return Token::Op; }
                int     { UpdateLoc(); return Token::Int; }
                
                [ \t\n]+ { UpdateLoc(); continue; } // Whitespace
                *       { UpdateLoc(); return Token::Error; } // Unknown Token
                $       { UpdateLoc(); return Token::End; } // End of Input
            */
        }
    }
    // NOLINTEND(cppcoreguidelines-avoid-goto)
}
