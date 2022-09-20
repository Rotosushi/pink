/**
 * @file Parser.h
 * @brief Header for class Parser
 * @version 0.1
 * 
 */
#pragma once
#include <iostream>

#include "aux/Error.h"   // pink::Error
#include "aux/Outcome.h" // pink::Outcome<>

#include "ast/Ast.h"     // pink::Ast
#include "type/Type.h"   // pink::Type

#include "front/Lexer.h" // pink::Lexer pink::Token

#include "aux/StringInterner.h" // pink::StringInterner

#include "ops/PrecedenceAndAssociativity.h" // pink::Precedence pink::Associativity

namespace pink {
    class Environment;

    /**
     * @brief Parser implements a LL(1) recursive [descent] parser for pink, with a separate operator [precedence] parser for binary operator terms.
     * 
     * [descent]: https://en.wikipedia.org/wiki/Recursive_descent_parser "descent"
     * [precedence]: https://en.wikipedia.org/wiki/Operator-precedence_parser "precedence"
     * 
     * The EBNF grammar the parser accepts is:
     * 
     * \verbatim
    
        top = function
            | bind
     
        bind = "var" id ":=" affix ";"
      
        function = "fn" id "(" [arg {"," arg}] ")" block
 
        arg = id ":" type

        block = "{" {term} "}"
     
        term = conditional
             | while
             | bind
             | affix ";"


        conditional = "if" "(" affix ")" block "else" block 
 
        while = "while" "(" affix ")" block

        affix = composite "=" affix                   
              | composite "(" [affix {"," affix}] ")" 
              | composite                             
 
        composite = dot operator infix-parser
                  | dot                       

        dot = basic {"." basic} // (1) (2)
    
        basic = id                        
              | integer                   
              | operator dot              
              | "true"                    
              | "false"                   
              | "(" affix {"," affix} ")" 
              | "[" affix {"," affix} "]" 
 
        type = "Int"
             | "Bool"
             | "(" type {"," type} ")"
             | "[" type "x" integer "]"
             | "ptr" type

        id = [a-zA-Z_][a-zA-Z0-9_]* // <- these are the regular expressions used for re2c
        operator = [*+\-/%<=>&|\^!~@$]+;
        integer = [0-9]+
     
        \endverbatim
     */
    class Parser {
    private:
        /**
         * @brief The Lexer used to translate input text into Tokens.
         * 
         */
        Lexer         lexer;

        /**
         * @brief The current Token being processed
         * 
         */
        Token         tok;

        /**
         * @brief The textual location of the current Token
         * 
         */
        Location      loc;

        /**
         * @brief The text which caused the Lexer to return the current Token
         * 
         */
        std::string   txt;

        /**
         * @brief The input stream to pull more input from.
         * 
         */
        std::istream* input_stream;

        /**
         * @brief Called to append another line of source text to the processing buffer.
         * 
         * Calling this procedure is handled by nexttok
         */
        void yyfill();

        /**
         * @brief Get the next Token from the input stream.
         * 
         * stores the next Token in tok, as well as filling 
         * loc and txt with their data.
         */
        void nexttok();

        /**
         * @brief Get a line of text from the input stream
         * 
         * acts exactly as std::getline, filling the buffer passed
         * from the input stream until it encounters the newline char '\\n'
         * except that unlike std::getline it appends the '\\n' 
         * to the buffer. This is to preserve the '\\n' for the 
         * lexer in order for the lexer to correctly count it's
         * position within the source file.
         * 
         * @param str the string to fill with the new line
         */
        void Getline(std::string& str);

        /**
         * @brief Parses Top level expressions
         * 
         * top = function
         *     | bind
         * 
         * @param env The environment associated with this compilation unit
         * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression which was parsed.
         * if false, then the Error which was encountered. 
         */
        Outcome<std::unique_ptr<Ast>, Error> ParseTop(const Environment& env);

        /**
         * @brief Parses Function expressions
         * 
         * function = "fn" id "(" [arg {"," arg}] ")" block
         * 
         * @param env The environment associated with this compilation unit
         * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression which was parsed.
         * if false, then the Error which was encountered. 
         */
        Outcome<std::unique_ptr<Ast>, Error> ParseFunction(const Environment& env);

        /**
         * @brief Parses Bind expressions
         * 
         * bind = "var" id ":=" affix ";"
         * 
         * @param env The environment associated with this compilation unit
         * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression which was parsed.
         * if false, then the Error which was encountered.  
         */
        Outcome<std::unique_ptr<Ast>, Error> ParseBind(const Environment& env);

        /**
         * @brief Parses Argument expressions
         * 
         * arg = id ":" type
         * 
         * @param env The environment associated with this compilation unit
         * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression which was parsed.
         * if false, then the Error which was encountered. 
         */
        Outcome<std::pair<InternedString, Type*>, Error> ParseArgument(const Environment& env);

        /**
         * @brief Parses Block expressions
         * 
         * block = "{" {term} "}"
         * 
         * @param env The environment associated with this compilation unit
         * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression which was parsed.
         * if false, then the Error which was encountered. 
         */
        Outcome<std::unique_ptr<Ast>, Error> ParseBlock(const Environment& env);

        /**
         * @brief Parses Term expressions
         * 
         * term = conditional
         *      | while
         *      | bind
         *      | affix ";"
         * 
         * @param env The environment associated with this compilation unit
         * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression which was parsed.
         * if false, then the Error which was encountered. 
         */
        Outcome<std::unique_ptr<Ast>, Error> ParseTerm(const Environment& env);

        /**
         * @brief Parses Conditional expressions
         * 
         * conditional = "if" "(" affix ")" block "else" block 
         * 
         * @param env The environment associated with this compilation unit
         * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression which was parsed.
         * if false, then the Error which was encountered.  
         */
        Outcome<std::unique_ptr<Ast>, Error> ParseConditional(const Environment& env);

        /**
         * @brief Parses While expressions
         * 
         * while = "while" "(" affix ")" block
         * 
         * @param env The environment associated with this compilation unit
         * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression which was parsed.
         * if false, then the Error which was encountered.  
         */
        Outcome<std::unique_ptr<Ast>, Error> ParseWhile(const Environment& env);

        /**
         * @brief Parses Affix expressions
         * 
         * affix = composite "=" affix               
         *       | composite "(" [affix {"," affix}] ")"
         *       | composite                            
         * 
         * @param env The environment associated with this compilation unit
         * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression which was parsed.
         * if false, then the Error which was encountered.
         */
        Outcome<std::unique_ptr<Ast>, Error> ParseAffix(const Environment& env);

        /**
         * @brief Parses Composite expressions
         * 
         * composite = dot operator infix-parser // <- this is actually a call to the op-prec parser
         *           | dot                       
         * 
         * @param env The environment associated with this compilation unit
         * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression which was parsed.
         * if false, then the Error which was encountered.
         */
        Outcome<std::unique_ptr<Ast>, Error> ParseComposite(const Environment& env);

        /**
         * @brief Parses Dot expressions
         * 
         * dot = basic {"." basic}
         * 
         * @param env The environment associated with this compilation unit
         * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression which was parsed.
         * if false, then the Error which was encountered.
         */
        Outcome<std::unique_ptr<Ast>, Error> ParseDot(const Environment& env);

        /**
         * @brief Parses Infix expressions
         * 
         * This is an implementation of an [Operator] Precedence Parser
         * [Operator]: https://en.wikipedia.org/wiki/Operator-precedence_parser "precedence"
         * 
         * @param left the first left hand side term of the binary operator expression
         * @param precedence the initial precedence to parse against, an initial call to this expression should pass 0 here.
         * @param env The environment associated with this compilation unit
         * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression which was parsed.
         * if false, then the Error which was encountered. 
         */
        Outcome<std::unique_ptr<Ast>, Error> ParseInfix(std::unique_ptr<Ast> left, Precedence precedence, const Environment& env);

        /**
         * @brief Parses Basic expressions
         * 
         * basic = id                        
         *       | integer                   
         *       | operator dot              
         *       | "true"                    
         *       | "false"                   
         *       | "(" affix {"," affix} ")" 
         *       | "[" affix {"," affix} "]" 
         * 
         * @param env The environment associated with this compilation unit
         * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression which was parsed.
         * if false, then the Error which was encountered. 
         */
        Outcome<std::unique_ptr<Ast>, Error> ParseBasic(const Environment& env);

        /**
         * @brief Parses Type expressions
         * 
         * type = "Int"
         *      | "Bool"
         *      | "(" type {"," type} ")"
         *      | "[" type "x" int "]"
         *      | "ptr" type
         * 
         * @param env The environment associated with this compilation unit
         * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression which was parsed.
         * if false, then the Error which was encountered.  
         */
        Outcome<Type*, Error> ParseType(const Environment& env);
    public:
        /**
         * @brief Construct a new Parser
         * 
         * the default initialization of the input stream is std::cin
         */
        Parser();

        /**
         * @brief Construct a new Parser
         * 
         * @param input_stream the input stream to read from, *CANNOT* be nullptr.
         */
        Parser(std::istream* input_stream);

        /**
         * @brief Destroy the Parser
         * 
         */
        ~Parser();
       
        /**
         * @brief Test if the Parser is at the end of it's input
         * 
         * @return true if the Lexer is not at the end of it's buffer
         * @return false if the Lexer is at the end of it's buffer
         */
        bool EndOfInput(); 

        /**
         * @brief Get the Buf
         * 
         * @return const std::string& The Lexer's buffer
         */
        const std::string& GetBuf();

        /**
         * @brief Get the input stream
         * 
         * @return std::istream* the input stream of the parser
         */
        std::istream* GetIStream();

        /**
         * @brief Set the input stream
         * 
         * @param input_stream the input stream to set. *CANNOT* be nullptr
         */
        void SetIStream(std::istream* input_stream);
        
        /**
         * @brief The entry point of the LL(1) Parser
         * 
         * @param env The environment associated with this compilation unit
         * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression which was parsed.
         * if false, then the Error which was encountered.
         */
        Outcome<std::unique_ptr<Ast>, Error> Parse(const Environment& env);
    };
}
