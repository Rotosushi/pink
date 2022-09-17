/**
 * @file Lexer.h
 * @brief header for the Lexer class
 * @version 0.1
 * 
 */
#pragma once
#include <string>

#include "aux/Location.h"
#include "front/Token.h"

namespace pink {
    /**
     * @brief the Lexer converts an input sequence of characters into an output sequence of [Token](#Token)s
     * 
     * The source code of the Lexer itself is generated by [re2c] from an re2c file describing the
     * operation of the Lexer in regular expressions. ()
     * 
     * [re2c]: https://re2c.org/manual/manual_c.html
     */
    class Lexer {
    private:
        /**
         * @brief The textual Location of the Lexer
         * 
         * the Location of the lexer starts at (1, 0, 1, 0)
         */
        Location     loc;

        /**
         * @brief The buffer holding the sequence of input characters to be Lexed
         * 
         */
        std::string  buffer;

        /**
         * @brief iterator to the end of the buffer being considered
         * 
         */
        std::string::iterator end;

        /**
         * @brief iterator to the end of the current token being lexed
         * 
         * that is it points to the end of the current token after a call 
         * to yylex, and during a call to yylex it is walked along the character
         * array to lex the token.
         */
        std::string::iterator cursor;

        /**
         * @brief iterator to the last place the iterator token was at in the buffer
         * 
         * this iterator is unused by our lexer because we do not make 
         * use of the lookahead features of re2c, nor do we make use of 
         * s-tags or m-tags while lexing.
         * 
         */
        std::string::iterator marker;

        /**
         * @brief iterator to the beginning of the current token within the buffer.
         * 
         */
        std::string::iterator token;

        /**
         * @brief updates the Location of the lexer to match the current token.
         * 
         * this is called after each token is lexed to keep the textual location
         * in sync with the iterator positions of the lexer.
         * 
         */
        void UpdateLoc();
    public:

        /**
         * @brief Construct a new Lexer.
         * 
         * the inital buffer is empty, and the initial location is (1, 0, 1, 0).
         */
        Lexer();

        /**
         * @brief Construct a new Lexer.
         * 
         * @param buf the new buffer contents.
         */
        Lexer(std::string& buf);

        /**
         * @brief Getter for Buf.
         * 
         * @return const std::string& the contents of the buffer.
         */
        const std::string& GetBuf();

        /**
         * @brief Setter for Buf.
         * 
         * @param buf the new buffer contents.
         */
        void SetBuf(std::string& buf);

        /**
         * @brief Appends the buffer with the given input text
         * 
         * This routine handles the possibility of iterator invalidation.
         * The buffer is extended to hold the new contents in addition to 
         * it's old contents.
         * 
         * @param text the text to append to the current buffer
         */
        void AppendBuf(const char* text);
        void AppendBuf(std::string& text);

        /**
         * @brief Reset the Lexer to a default constructed state.
         * 
         */
        void Reset();

        /**
         * @brief Get if the Lexer is at the end of it's buffer.
         * 
         * @return true if the distance between the cursor and the end is zero
         * @return false if the distance between the cursor and the end is non-zero
         */
        bool EndOfInput();

        /**
         * @brief Get the *next* Token from the buffer.
         * 
         * \warning This function *advances* the internal state of the Lexer to 
         * point to the next token from the buffer. The Lexer *does not* save the
         * previous Location, text, or Token it was looking at, so
         * be sure you have saved the current Token, text and Location if that
         * is necessary.
         * 
         * @return Token the *next* token from the buffer
         */
        Token       yylex();

        /**
         * @brief Get the text of the the *current* Token.
         * 
         * Calling this function *does not* advance the internal state of the Lexer.
         * So the text returned corresponds to the *current* token the Lexer is pointing to.
         * That is the *last* token it lexed.
         * 
         * @return std::string the text corresponding to the *current* token.
         */
        std::string yytxt();

        /**
         * @brief Get the Location of the *current* Token.
         * 
         * Calling this function *does not* advance the internal state of the Lexer.
         * So the Location returned corresponds to the *current* token the Lexer is pointing to.
         * That is the *last* token it lexed.
         * 
         * @return Location the Location of the *current* Token
         */
        Location    yyloc();
    };
}
