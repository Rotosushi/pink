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

#include "ast/Ast.h"            // pink::Ast
#include "type/TypeInterface.h" // pink::Type

#include "front/Lexer.h" // pink::Lexer pink::Token

#include "aux/StringInterner.h" // pink::StringInterner

#include "ops/PrecedenceAndAssociativity.h" // pink::Precedence pink::Associativity

namespace pink {
class Environment;

/**
 * @brief Parser implements a LL(1) recursive [descent] parser for the pink
language with a separate operator [precedence] parser for binary operator terms.
 *
 * [descent]: https://en.wikipedia.org/wiki/Recursive_descent_parser "descent"
 * [precedence]: https://en.wikipedia.org/wiki/Operator-precedence_parser
"precedence"
 *
 *
 * The EBNF grammar the parser accepts is:
 *
 * \verbatim

top = function
    | bind

bind = ["var"] id ":=" affix ";"

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

composite = accessor operator infix-parser
          | accessor

accessor = basic {"." basic}
         | basic {"[" basic "]"}
         | basic

basic = id [":=" affix]
      | integer
      | operator accessor
      | "nil"
      | "true"
      | "false"
      | "(" affix {"," affix} ")"
      | "[" affix {"," affix} "]"

type = "Nil"
     | "Integer"
     | "Boolean"
     | "(" type {"," type} ")"
     | "[" type ";" integer "]"
     | "Pointer" type

// these are the regular expressions used by re2c
id = [a-zA-Z_][a-zA-Z0-9_]*
operator = [*+\-/%<=>&|\^!~@$]+;
integer = [0-9]+

    \endverbatim
 */

/*
what about this EBNF grammar?

top = declaration ";"

declaration = bind
            | function
            | type

bind = identifier ":" [constness] [type] "=" affix ";"
     | identifier ":" [constness] [type] ["=" affix] ";"

constness = "var"
          | "const"

function = "fn" identifier argument-list block

affix = application
      | application binop infix-parser

binop = "+"
      | "-"
      | "*"
      | "/"
      | "%"
      | ">"
      | "<"
      | "="
      | "<="
      | ">="
      | "&"
      | "|"
      | "^"


application = basic {basic}

accessor = basic {("." affix)}

basic = identifier
      |
      | unop accessor
      | tuple_or_affix
      | array
      | conditional
      | loop
      | block
      | integer
      | "nil"
      | "true"
      | "false"
      | "(" affix ")"

unop = operator

tuple_or_affix = "(" affix {"," affix} ")"

array = "[" affix {"," affix} "]"

conditional = "if" "(" affix ")" (affix | block) ["else" (affix | block)]

loop = "while" "(" affix ")" (affix | block)

block = "{" affix {";" affix} "}"

type = "Nil"
     | "Integer"
     | "Boolean"
     | "Pointer" template
     | "Tuple" template
     | "Array" template
     | type {"->" type}

template = "<" type {"," type} ">"

identifier = [a-zA-Z_][a-zA-Z0-9_]*
integer = [0-9]+
*/

class Parser {
public:
  using Result     = Outcome<Ast::Pointer, Error>;
  using TypeResult = Outcome<Type::Pointer, Error>;

private:
  std::istream *input_stream;
  Lexer         lexer;

  Token            token;
  Location         location;
  std::string_view text;

  [[nodiscard]] auto Getline() -> std::string;

  /**
   * @brief Get the next Token from the input stream.
   *
   * stores the next Token in token, as well as filling
   * location and text with their data.
   */
  void nexttok();

  /**
   * @brief returns if the current token *was* token, advances parser state only
   * when *true*
   *
   * \warning This function calls [nexttok](#Parser::nexttok) if
   * [token](#Parser::token) *equals* token
   *
   * @param token the token to compare
   * @return true the current token *was* equal to token, advances parser state
   * @return false the current token *was not* equal to token, does not advance
   * parser state
   */
  auto Expect(Token token) -> bool;

  /**
   * @brief returns if the current token *was* token, does not advance parser
   * state
   *
   * @param token the token to compare
   * @return true token == token
   * @return false token != token
   */
  auto Peek(Token token) -> bool;

  /**
   * @brief Parses Top level expressions
   *
   * \verbatim
   * top = function
   *     | bind
   * \endverbatim
   *
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto ParseTop(Environment &env) -> Result;

  /**
   * @brief Parses Function expressions
   *
   * \verbatim
   * function = "fn" id "(" [arg {"," arg}] ")" block
   * \endverbatim
   *
   * \todo add an optional explicit return type
   * \todo allow functions to have a single body statement
   * followed by a semicolon instead of a block for their
   * body.
   *
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto ParseFunction(Environment &env) -> Result;

  /**
   * @brief Parses a Bind expression
   *
   * \verbatim
      bind = ["var"] id ":=" affix ";"
   * \endverbatim
   *
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto ParseBind(Environment &env) -> Result;

  /**
   * @brief Parses a Bind expression
   *
   * \verbatim
      bind = ["var"] id ":=" affix ";"
   * \endverbatim
   *
   * @param name the id in the expression
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto ParseBind(InternedString name, Location lhs_location, Environment &env)
      -> Result;

  /**
   * @brief Parses Argument expressions
   *
   * \verbatim
      arg = id ":" type
   * \endverbatim
   *
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto ParseArgument(Environment &env)
      -> Outcome<std::pair<InternedString, Type::Pointer>, Error>;
  /**
   * @brief Parses Block expressions
   *
   * \verbatim
      block = "{" {term} "}"
   * \endverbatim
   *
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto ParseBlock(Environment &env) -> Result;

  /**
   * @brief Parses Term expressions
   *
   * \verbatim
      term = conditional
           | while
           | bind
           | affix ";"
   * \endverbatim
   *
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto ParseTerm(Environment &env) -> Result;

  /**
   * @brief Parses IfThenElse expressions
   *
   * \verbatim
      conditional = "if" "(" affix ")" block "else" block
   * \endverbatim
   *
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto ParseConditional(Environment &env) -> Result;

  /**
   * @brief Parses While expressions
   *
   * \verbatim
      while = "while" "(" affix ")" block
   * \endverbatim
   *
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto ParseWhile(Environment &env) -> Result;

  /**
   * @brief Parses Affix expressions
   *
   * \verbatim
      affix = composite "=" affix
            | composite "(" [affix {"," affix}] ")"
            | composite
   * \endverbatim
   *
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto ParseAffix(Environment &env) -> Result;

  /**
   * @brief Parses Assignment expressions
   *
   * \verbatim
      assignment = composite "=" affix
   * \endverbatim
   *
   * @param env the environment associated with this compilation unit
   * @param composite the lhs of the assignment
   * @return Outcome<std::unique_ptr<Ast>, Error> if true then the expression,
   * if false then the Error which was encountered.
   */
  auto ParseAssignment(Ast::Pointer composite, Environment &env) -> Result;

  /**
   * @brief Parses an Application expression
   *
   * \verbatim
      application = composite "(" [affix {"," affix}] ")"
   * \endverbatim
   *
   * @param env the environment associated with this compilation unit
   * @param composite the callee of the application
   * @return Outcome<std::unique_ptr<Ast>, Error> if true then the expression,
   * if false then the Error which was encountered.
   */
  auto ParseApplication(Ast::Pointer composite, Environment &env) -> Result;

  /**
   * @brief Parses Composite expressions
   *
   * \verbatim
      composite = accessor operator infix-parser
                | accessor
   * \endverbatim
   *
   * \note infix-parser is actually the call into the operator precedence
   * parser, meaning this one line consumes the entire infix expression.
   *
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto ParseComposite(Environment &env) -> Result;

  /**
   * @brief Parses accessor expressions
   *
   * \verbatim
      accessor = basic {"." basic}
               | basic {"[" basic "]"}
               | basic
   * \endverbatim
   *
   * this function is here to give accessor operators a higher
   * operator precedence than any possible operator.
   * so that any actual binary operations can use the
   * values which were accessed, instead of operating on the
   * composite structures themselves.
   *
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto ParseAccessor(Environment &env) -> Result;

  /**
   * @brief Parses member access expressions
   *
   * \verbatim
      dot = basic {"." basic}
   * \endverbatim
   *
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto ParseDot(Ast::Pointer left, Environment &env) -> Result;

  /**
   * @brief Parses subscript access expressions
   *
   * \verbatim
      subscript = basic {"[" basic "]"}
   * \endverbatim
   *
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto ParseSubscript(Ast::Pointer left, Environment &env) -> Result;

  /**
   * @brief Parses Infix expressions
   *
   * This is an implementation of an [Operator] Precedence Parser
   * [Operator]: https://en.wikipedia.org/wiki/Operator-precedence_parser
   *
   *
   * @param left the first left hand side term of the binary operator expression
   * @param precedence the initial precedence to parse against, an initial call
   * to this expression must pass 0 here.
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto ParseInfix(std::unique_ptr<Ast> left,
                  Precedence           precedence,
                  Environment         &env) -> Result;

  /**
   * @brief Parses Basic expressions
   *
   * \verbatim
      basic = id [":=" affix]
            | integer
            | operator accessor
            | "true"
            | "false"
            | "(" affix {"," affix} ")"
            | "[" affix {"," affix} "]"
   * \endverbatim
   *
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto ParseBasic(Environment &env) -> Result;

  /**
   * @brief Parses a Tuple
   *
   * \verbatim
      "(" affix {"," affix} ")"
   * \endverbatim
   *
   * \note assuming that we start from the comma
   *
   * @param env the environment associated with this compilation unit
   * @param first_element the first element of the tuple, already parsed
   * @return Outcome<std::unique_ptr<Ast>, Error> if true then the expression,
   * if false then the Error which was encountered.
   */
  auto ParseTuple(Ast::Pointer first_element, Environment &env) -> Result;

  /**
   * @brief Parses an Array
   *
   * \verbatim
      "[" affix {"," affix} "]"
   * \endverbatim
   *
   * @param env the environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true then the expression,
   * if false then the Error which was encountered.
   */
  auto ParseArray(Environment &env) -> Result;

  /**
   * @brief Parses Type expressions
   *
   * \verbatim
      type = "Integer"
           | "Boolean"
           | "Pointer" type
           | "Slice" type
           | "(" type {"," type} ")"
           | "[" type ";" int "]"

   * \endverbatim
   *
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto ParseType(Environment &env) -> TypeResult;

  auto ParseArrayType(Environment &env) -> TypeResult;
  auto ParseTupleType(Environment &env) -> TypeResult;
  auto ParsePointerType(Environment &env) -> TypeResult;
  auto ParseSliceType(Environment &env) -> TypeResult;

public:
  Parser();
  Parser(std::istream *input_stream);
  ~Parser()                                       = default;
  Parser(const Parser &other)                     = delete;
  Parser(Parser &&other)                          = default;
  auto operator=(const Parser &other) -> Parser & = delete;
  auto operator=(Parser &&other) -> Parser      & = default;

  /**
   * @brief Test if the Parser is at the end of it's input
   *
   * @return true if the Lexer is not at the end of it's buffer
   * @return false if the Lexer is at the end of it's buffer
   */
  [[nodiscard]] auto EndOfInput() const -> bool;

  /**
   * @brief Extracts the line of text specified by the Location given.
   *
   * Returns the string containing the line of text, starting from
   * [firstLine](#Location::firstLine) till the next newline character,
   * from the buffer held by the Lexer.
   *
   *
   * \note this function only works because we buffer the -entire- file as
   * we read it. Were we to use a fixed buffer which slides in more text
   * as it is read, this routine would need to reparse the input file from
   * the beginning to retrieve the source line of code. in the case that any
   * text had been slid out of the buffer to make room.
   *
   * \note this function takes O(N) time, because it actually has to scan the
   * buffer until it gets to the line
   *
   * @param location the location to search for
   * @return std::string the line we were searching for, or if that couldn't
   * be found then the empty string
   */
  [[nodiscard]] auto ExtractSourceLine(const Location &location) const
      -> std::string_view;

  /**
   * @brief Get the input stream
   *
   * @return std::istream* the input stream of the parser
   */
  [[nodiscard]] auto GetIStream() const -> std::istream *;

  /**
   * @brief Set the input stream
   *
   * @param input_stream the input stream to set. *CANNOT* be nullptr
   */
  void SetIStream(std::istream *input_stream);

  /**
   * @brief appends text directly to the end of the buffer
   * of the parser.
   *
   * @param text
   */
  void AppendToBuffer(std::string_view text) { lexer.AppendToBuffer(text); }

  void SetBuffer(std::string_view text) { lexer.SetBuffer(text); }
  /**
   * @brief The entry point of the LL(1) Parser
   *
   * \note This function is O(N) because it is LL(1)
   * which boils down to the fact that it never has
   * to reparse any of the input it reads.
   *
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto Parse(Environment &env) -> Result;
};
} // namespace pink
