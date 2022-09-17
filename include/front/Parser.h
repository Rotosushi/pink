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
     * term = affix ";"
     * 
     * affix = function
     *       | composite [operator infix-parser] // <- infix-parser is actually a call to the op-prec parser
     * 
     * composite = dot
     *           | dot "=" affix
     *           | dot "(" [affix {"," affix}] ")"
     * 
     * dot = basic
     *     | basic '.' affix
     * 
     * basic = id [":=" affix]
     *       | operator basic
     *       | "(" affix {"," affix} ")"
     *       | "[" affix {"," affix} "]"
     *       | "nil"
     *       | int
     *       | "true"
     *       | "false"
     *       | conditional
     *       | while
     * 
     * function = "fn" id "(" [arg {"," arg}] ")" block
     * 
     * arg = id ":" type
     * 
     * block = "{" {term} "}"
     * 
     * conditional = "if" affix "then" block "else" block
     * 
     * while = "while" affix "do" block
     * 
     * type = basicType [*]
     * 
     * basicType = "Nil"
     *           | "Int"
     *           | "Bool"
     * 
     * id = [a-zA-Z_][a-zA-Z0-9_]* // <- this is the regular expression used for re2c
     * operator = [*+\-/%<=>&|\^!~@$]+; // <- this is the regular expression used for re2c
     * int = [0-9]+
     * 
     * -------------------------------------
     * 
     * after updating the EBNF to exactly what the parser accepts,
     * I think it would be better as: 
     * 
     * top = function
     *     | variable
     * 
     * variable = "var" id ":=" affix ";"
     *       
     * function = "fn" id "(" [arg {"," arg}] ")" block
     * 
     * arg = id ":" type
     * 
     * block = "{" {term} "}"
     * 
     * term = conditional
     *      | while
     *      | variable
     *      | affix ";"
     * 
     * conditional = "if" "(" affix ")" block "else" block
     * 
     * while = "while" "(" affix ")" block
     * 
     * affix = infix "=" affix
     *       | infix "(" [affix {"," affix}] ")"
     *       | infix
     * 
     * infix = dot [operator infix-parser] // <- this is actually a call to the op-prec parser
     * 
     * dot = basic ["." dot]
     * 
     * basic = id
     *       | integer
     *       | "true"
     *       | "false"
     *       | "(" affix {"," affix} ")"
     *       | "[" affix {"," affix} "]"
     * 
     * type = basic_type
     * 
     * basic_type = "Int"
     *            | "Bool"
     *            | "(" type {"," type} ")"
     *            | "[" type "x" int "]"
     *            | "ptr" type
     * 
     */
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

        Outcome<std::unique_ptr<Ast>, Error> ParseTerm(const Environment& env);
        Outcome<std::unique_ptr<Ast>, Error> ParseAffix(const Environment& env);
        Outcome<std::unique_ptr<Ast>, Error> ParseComposite(const Environment& env);
        Outcome<std::unique_ptr<Ast>, Error> ParseDot(const Environment& env);
        Outcome<std::unique_ptr<Ast>, Error> ParseInfix(std::unique_ptr<Ast> left, Precedence precedence, const Environment& env);
        Outcome<std::unique_ptr<Ast>, Error> ParseBasic(const Environment& env);
        Outcome<std::unique_ptr<Ast>, Error> ParseBlock(const Environment& env);
        Outcome<std::unique_ptr<Ast>, Error> ParseConditional(const Environment& env);
        Outcome<std::unique_ptr<Ast>, Error> ParseWhile(const Environment& env);
        Outcome<std::unique_ptr<Ast>, Error> ParseFunction(const Environment& env);
        Outcome<std::pair<InternedString, Type*>, Error> ParseArgument(const Environment& env);
        Outcome<Type*, Error> ParseType(const Environment& env);
        Outcome<Type*, Error> ParseBasicType(const Environment& env);
    public:
        Parser(); // by default the parser reads input from std::cin
        Parser(std::istream* input_stream);
        ~Parser();
       
        bool EndOfInput(); 
        const std::string& GetBuf();
        std::istream* GetIStream();
        void SetIStream(std::istream* input_stream);
        // acts exactly as std::getline, filling the buffer passed
        // from the input stream until it encounters the newline char '\n'
        // except that unlike std::getline it appends the '\n' 
        // to the buffer. This is to preserve the '\n' for the 
        // lexer in order for the lexer to correctly count it's
        // position within the source file.
        void Getline(std::string& str);
        
        Outcome<std::unique_ptr<Ast>, Error> Parse(const Environment& env);
    };
}
