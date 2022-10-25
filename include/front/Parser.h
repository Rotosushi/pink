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

#include "ast/Ast.h"   // pink::Ast
#include "type/Type.h" // pink::Type

#include "front/Lexer.h" // pink::Lexer pink::Token

#include "aux/StringInterner.h" // pink::StringInterner

#include "ops/PrecedenceAndAssociativity.h" // pink::Precedence pink::Associativity

namespace pink {
class Environment;

/**
 * @brief Parser implements a LL(1) recursive [descent] parser for pink, with a
 separate operator [precedence] parser for binary operator terms.
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

composite = accessor operator infix-parser
          | accessor

accessor = basic {"." basic}
         | basic {"[" basic "]"}
         | basic

basic = id
      | integer
      | operator accessor
      | "true"
      | "false"
      | "(" affix {"," affix} ")"
      | "[" affix {"," affix} "]"

type = "Int"
     | "Bool"
     | "(" type {"," type} ")"
     | "[" type "x" integer "]"
     | "ptr" type

// these are the regular expressions used by re2c
id = [a-zA-Z_][a-zA-Z0-9_]*
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
  Lexer lexer;

  /**
   * @brief The current Token being processed
   *
   */
  Token tok;

  /**
   * @brief The textual location of the current Token
   *
   */
  Location loc;

  /**
   * @brief The text which caused the Lexer to return the current Token
   *
   */
  std::string txt;

  /**
   * @brief The input stream to pull more input from.
   *
   */
  std::istream *input_stream;

  /**
   * @brief Called to append another line of source text to the processing
   * buffer.
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
   * @brief returns if the current token *was* token, advances parser state only
   * when *true*
   *
   * \warning This function calls [nexttok](#Parser::nexttok) if
   * [tok](#Parser::tok) *equals* token
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
   * @return true tok == token
   * @return false tok != token
   */
  auto Peek(Token token) -> bool;

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
  [[nodiscard]] auto Getline() const -> std::string;

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
  auto ParseTop(const Environment &env) -> Outcome<std::unique_ptr<Ast>, Error>;

  /**
   * @brief Parses Function expressions
   *
   * \verbatim
   * function = "fn" id "(" [arg {"," arg}] ")" block
   * \endverbatim
   *
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto ParseFunction(const Environment &env)
      -> Outcome<std::unique_ptr<Ast>, Error>;

  /**
   * @brief Parses Bind expressions
   *
   * \verbatim
   * bind = "var" id ":=" affix ";"
   * \endverbatim
   *
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto ParseBind(const Environment &env)
      -> Outcome<std::unique_ptr<Ast>, Error>;

  /**
   * @brief Parses Argument expressions
   *
   * \verbatim
   * arg = id ":" type
   * \endverbatim
   *
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto ParseArgument(const Environment &env)
      -> Outcome<std::pair<InternedString, Type *>, Error>;

  /**
   * @brief Parses Block expressions
   *
   * \verbatim
   * block = "{" {term} "}"
   * \endverbatim
   *
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto ParseBlock(const Environment &env)
      -> Outcome<std::unique_ptr<Ast>, Error>;

  /**
   * @brief Parses Term expressions
   *
   * \verbatim
   * term = conditional
   *      | while
   *      | bind
   *      | affix ";"
   * \endverbatim
   *
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto ParseTerm(const Environment &env)
      -> Outcome<std::unique_ptr<Ast>, Error>;

  /**
   * @brief Parses Conditional expressions
   *
   * \verbatim
   * conditional = "if" "(" affix ")" block "else" block
   * \endverbatim
   *
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto ParseConditional(const Environment &env)
      -> Outcome<std::unique_ptr<Ast>, Error>;

  /**
   * @brief Parses While expressions
   *
   * \verbatim
   * while = "while" "(" affix ")" block
   * \endverbatim
   *
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto ParseWhile(const Environment &env)
      -> Outcome<std::unique_ptr<Ast>, Error>;

  /**
   * @brief Parses Affix expressions
   *
   * \verbatim
   * affix = composite "=" affix
   *       | composite "(" [affix {"," affix}] ")"
   *       | composite
   * \endverbatim
   *
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto ParseAffix(const Environment &env)
      -> Outcome<std::unique_ptr<Ast>, Error>;

  /**
   * @brief Parses Assignment expressions
   *
   * \verbatim
   * assignment = composite "=" affix
   * \endverbatim
   *
   * @param env the environment associated with this compilation unit
   * @param composite the lhs of the assignment
   * @return Outcome<std::unique_ptr<Ast>, Error> if true then the expression,
   * if false then the Error which was encountered.
   */
  auto ParseAssignment(const Environment &env, std::unique_ptr<Ast> composite)
      -> Outcome<std::unique_ptr<Ast>, Error>;

  /**
   * @brief Parses an Application expression
   *
   * \verbatim
   * application = composite "(" [affix {"," affix}] ")"
   * \endverbatim
   *
   * @param env the environment associated with this compilation unit
   * @param composite the callee of the application
   * @return Outcome<std::unique_ptr<Ast>, Error> if true then the expression,
   * if false then the Error which was encountered.
   */
  auto ParseApplication(const Environment &env, std::unique_ptr<Ast> composite)
      -> Outcome<std::unique_ptr<Ast>, Error>;

  /**
   * @brief Parses Composite expressions
   *
   * \verbatim
   * composite = accessor operator infix-parser
   *           | accessor
   * \endverbatim
   *
   * \note infix-parser is actually the call into the operator precedence
   * parser, meaning this one line consumes the entire infix expression.
   *
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto ParseComposite(const Environment &env)
      -> Outcome<std::unique_ptr<Ast>, Error>;

  /**
   * @brief Parses accessor expressions
   *
   * \verbatim
   * accessor = basic {"." basic}
   *          | basic {"[" basic "]"}
   *          | basic
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
  auto ParseAccessor(const Environment &env)
      -> Outcome<std::unique_ptr<Ast>, Error>;

  /**
   * @brief Parses member access expressions
   *
   * \verbatim
   * dot = basic {"." basic}
   * \endverbatim
   *
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto ParseDot(const Environment &env, std::unique_ptr<Ast> left)
      -> Outcome<std::unique_ptr<Ast>, Error>;

  /**
   * @brief Parses subscript access expressions
   *
   * \verbatim
   * subscript = basic {"[" basic "]"}
   * \endverbatim
   *
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto ParseSubscript(const Environment &env, std::unique_ptr<Ast> left)
      -> Outcome<std::unique_ptr<Ast>, Error>;

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
  auto ParseInfix(std::unique_ptr<Ast> left, Precedence precedence,
                  const Environment &env)
      -> Outcome<std::unique_ptr<Ast>, Error>;

  /**
   * @brief Parses Basic expressions
   *
   * \verbatim
   * basic = id
   *       | integer
   *       | operator accessor
   *       | "true"
   *       | "false"
   *       | "(" affix {"," affix} ")"
   *       | "[" affix {"," affix} "]"
   * \endverbatim
   *
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto ParseBasic(const Environment &env)
      -> Outcome<std::unique_ptr<Ast>, Error>;

  /**
   * @brief Parses a Tuple
   *
   * \verbatim
   *  "(" affix {"," affix} ")"
   * \endverbatim
   *
   * \note assuming that we start from the comma
   *
   * @param env the environment associated with this compilation unit
   * @param first_element the first element of the tuple, already parsed
   * @return Outcome<std::unique_ptr<Ast>, Error> if true then the expression,
   * if false then the Error which was encountered.
   */
  auto ParseTuple(const Environment &env, std::unique_ptr<Ast> first_element)
      -> Outcome<std::unique_ptr<Ast>, Error>;

  /**
   * @brief Parses an Array
   *
   * \verbatim
   * "[" affix {"," affix} "]"
   * \endverbatim
   *
   * @param env the environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true then the expression,
   * if false then the Error which was encountered.
   */
  auto ParseArray(const Environment &env)
      -> Outcome<std::unique_ptr<Ast>, Error>;

  /**
   * @brief Parses Type expressions
   *
   * \verbatim
   * type = "Int"
   *      | "Bool"
   *      | "(" type {"," type} ")"
   *      | "[" type "x" int "]"
   *      | "ptr" type
   * \endverbatim
   *
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto ParseType(const Environment &env) -> Outcome<Type *, Error>;

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
   * @param input_stream the input stream; *CANNOT* be nullptr.
   */
  Parser(std::istream *input_stream);

  /**
   * @brief Destroy the Parser
   *
   */
  ~Parser() = default;

  Parser(const Parser &other) = delete;

  Parser(Parser &&other) = delete;

  auto operator=(const Parser &other) = delete;

  auto operator=(Parser &&other) = delete;

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
   * \note this function could also return the string from firstLine
   * to the end of lastLine, but I don't know which behavior is more
   * useful for the error reporting function.
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
   * @param loc the location to search for
   * @return std::string the line we were searching for, or if that couldn't
   * be found then the empty string
   */
  [[nodiscard]] auto ExtractLine(const Location &loc) const -> std::string;

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
   * @brief The entry point of the LL(1) Parser
   *
   * \note This function is O(N) because it is LL(1)
   * which boils down to the fact that it never has
   * to reparse any of the input it reads.
   *
   * \note this class would be easy to adapt to be able to
   * reparse it's input if it buffered all of the tokens that
   * it read
   *
   * @param env The environment associated with this compilation unit
   * @return Outcome<std::unique_ptr<Ast>, Error> if true, then the expression
   * which was parsed. if false, then the Error which was encountered.
   */
  auto Parse(const Environment &env) -> Outcome<std::unique_ptr<Ast>, Error>;
};
} // namespace pink
