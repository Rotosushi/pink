// Copyright (C) 2023 cadence
//
// This file is part of pink.
//
// pink is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// pink is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with pink.  If not, see <http://www.gnu.org/licenses/>.

#include "front/Parser.h"

#include "aux/Environment.h"

// Syntactic Forms
#include "ast/All.h"

// Types
#include "type/All.h"

#include "support/ToNumber.h"

// This is a convenience macro for a common usage
// pattern of the Outcome type.
// expresses immediate return on error.
// We cannot replace this with a template function,
// as we need the return statement to be in the scope
// this snippet of code is inserted into.
// and we cannot use this pattern directly in the visitor functions
// elsewhere as these do not return values normally. so
// the macro is defined in this file only.
// admittedly this is kinda hacky.
#define TRY(outcome, variable, function, ...)                                  \
  auto outcome = function(__VA_ARGS__);                                        \
  if (!outcome) {                                                              \
    return outcome.GetSecond();                                                \
  }                                                                            \
  auto &variable = outcome.GetFirst();

namespace pink {
Parser::Parser()
    : input_stream(&std::cin),
      token(Token::End),
      location(1, 0, 1, 0) {}

Parser::Parser(std::istream *input_stream)
    : input_stream(input_stream),
      token(Token::End),
      location(1, 0, 1, 0) {
  assert(input_stream != nullptr);
}

auto Parser::GetIStream() const -> std::istream * { return input_stream; }

void Parser::SetIStream(std::istream *stream) {
  assert(stream != nullptr);
  input_stream = stream;
}

auto Parser::EndOfInput() const -> bool {
  return lexer.EndOfInput() && input_stream->eof();
}

auto Parser::Getline() -> std::string {
  std::string buffer;
  char        character = '\0';

  do {
    character = static_cast<char>(input_stream->get());

    if (input_stream->eof()) {
      break;
    }

    buffer += character;
  } while (character != '\n');
  return buffer;
}

void Parser::nexttok() {
  token    = lexer.lex(); // this statement advances the lexer's internal state.
  location = lexer.loc();
  text     = lexer.txt();

  while (lexer.EndOfInput() && !input_stream->eof()) {
    auto line = Getline();
    lexer.AppendToBuffer(line);
    token    = lexer.lex();
    location = lexer.loc();
    text     = lexer.txt();
  }
}

auto Parser::Expect(Token expected) -> bool {
  if (token == expected) {
    nexttok();
    return true;
  }

  return false;
}

auto Parser::Peek(Token expected) -> bool { return token == expected; }

auto Parser::IsBinop(Token token) -> bool {
  switch (token) {
  case Token::Add:
  case Token::Sub:
  case Token::Star:
  case Token::Divide:
  case Token::Modulo:
  case Token::And:
  case Token::Or:
  case Token::Equals:
  case Token::NotEquals:
  case Token::LessThan:
  case Token::LessThanOrEqual:
  case Token::GreaterThan:
  case Token::GreaterThanOrEqual:
    return true;
  default:
    return false;
  }
}

/*
  precedence table:
      == !=     : 1
      < <= > >= : 2
      & |       : 3
      + -       : 4
      * / %     : 5

  #REASON
  in short, I want [a + b * c == d * e + f]
  to parse as [(a + (b * c)) == ((c * d) + f)]
  to ensure that comparisons happen on the
  'most evaluated' values. The only pain point is
  that if you want to combine the comparison expressions
  themselves you must use parenthesis, however I feel like
  that is a small price to pay, when it makes boolean
  conjunction/disjunction occur before comparison by default.
*/
auto Parser::Precedence(Token token) -> pink::Precedence {
  switch (token) {
  case Token::Equals:
  case Token::NotEquals:
    return 1;
  case Token::LessThan:
  case Token::LessThanOrEqual:
  case Token::GreaterThan:
  case Token::GreaterThanOrEqual:
    return 2;
  case Token::And:
  case Token::Or:
    return 3;
  case Token::Add:
  case Token::Sub:
    return 4;
  case Token::Star:
  case Token::Divide:
  case Token::Modulo:
    return 5;
  default:
    return 0;
  }
}

auto Parser::Associativity(Token token) -> pink::Associativity {
  switch (token) {
  case Token::Add:
  case Token::Sub:
  case Token::Star:
  case Token::Divide:
  case Token::Modulo:
  case Token::And:
  case Token::Or:
  case Token::Equals:
  case Token::NotEquals:
  case Token::LessThan:
  case Token::LessThanOrEqual:
  case Token::GreaterThan:
  case Token::GreaterThanOrEqual:
    return Associativity::Left;
  default:
    return Associativity::None;
  }
}

/*
  Extract the n'th line of source text from the buffer,
  where n = location.firstLine
*/
auto Parser::ExtractSourceLine(const Location &location) const
    -> std::string_view {
  const auto  buf    = lexer.GetBufferView();
  const auto *cursor = buf.begin();
  const auto *end    = buf.end();

  size_t lines_seen = 1;

  while ((lines_seen < location.firstLine) && (cursor != end)) {
    if (*cursor == '\n') {
      lines_seen++;
    }

    cursor++;
  }

  if (cursor != end) {
    const auto *eol = cursor;

    while (eol != end && *eol != '\n') {
      eol++;
    }
    return {cursor, eol};
  }
  return {};
}

auto Parser::Parse(CompilationUnit &env) -> Parser::Result {
  // prime the lexer with the first token from the input stream;
  // only if we are not already parsing an input stream.
  if (Peek(Token::End) && lexer.EndOfInput() && !input_stream->eof()) {
    nexttok();
  }

  // if there is no more source return EndOfFile.
  if (Peek(Token::End) && input_stream->eof()) {
    return {Error(Error::Code::EndOfFile, location)};
  }

  return ParseTop(env);
}

/*
  top = function
      | bind ';'
*/
auto Parser::ParseTop(CompilationUnit &env) -> Parser::Result {
  if (Peek(Token::Fn)) {
    return ParseFunction(env);
  }

  if (Peek(Token::Id) || Peek(Token::Var)) {
    auto outcome = ParseBind(env);
    if (!outcome) {
      return outcome;
    }

    if (!Expect(Token::Semicolon)) {
      return {Error(Error::MissingSemicolon, location, text)};
    }

    return outcome;
  }
  /// \note  is it worth it to attempt to parse the bad expression
  /// such that we don't leave the parser at the error we encountered?
  /// as it stands now we can only handle a single parse error before
  /// we bail. if we wanted to display more than the first error
  /// we encountered, we would need to do something along these lines.
  /// otherwise when we reentered the parser we will parse the exact
  /// same error, as if we were entering the parse tree from the beginning.
  return {
      Error(Error::Code::BadTopLevelExpression,
            location,
            "only variables and functions may be declared at global scope")};
}

/*
  bind = ("var")? id ":=" affix ";"
*/
auto Parser::ParseBind(CompilationUnit &env) -> Parser::Result {
  Location lhs_loc = location;

  if (Peek(Token::Var)) {
    nexttok(); // eat 'var'
  }

  if (!Peek(Token::Id)) {
    return Error(Error::Code::MissingBindId, location, text);
  }

  const auto *name = env.InternVariable(text);

  nexttok(); // eat id

  return ParseBind(name, lhs_loc, env);
}

auto Parser::ParseBind(InternedString   name,
                       Location         lhs_location,
                       CompilationUnit &env)
    -> Outcome<std::unique_ptr<Ast>, Error> {
  if (!Expect(Token::ColonEq)) {
    return Error(Error::Code::MissingBindColonEq, location, text);
  }

  TRY(affix_result, affix, ParseAffix, env)

  Location bind_loc(lhs_location.firstLine,
                    lhs_location.firstColumn,
                    location.lastLine,
                    location.lastColumn);
  return {std::make_unique<Bind>(bind_loc, name, std::move(affix))};
}

/*
  function = "fn" id "(" [arg {"," arg}] ")" block
*/
auto Parser::ParseFunction(CompilationUnit &env) -> Parser::Result {
  Function::Arguments args;
  Location            lhs_loc = location;

  nexttok(); // eat 'fn'

  if (!Peek(Token::Id)) {
    return Error(Error::Code::MissingFnName, location, text);
  }

  auto name = env.InternVariable(text); // intern the functions name

  nexttok(); // eat 'id'

  if (!Expect(Token::LParen)) {
    return Error(Error::Code::MissingLParen, location, text);
  }

  // #RULE a functions argument list is optional
  if (Peek(Token::Id)) {
    do {
      TRY(arg_result, arg, ParseArgument, env)

      args.emplace_back(std::move(arg));
    } while (Expect(Token::Comma));
  }

  if (!Expect(Token::RParen)) {
    return Error(Error::Code::MissingRParen, location, text);
  }

  // #PROPOSAL we could easily parse functions which have no
  // block expression if we allow the alternative
  // to be [affix ';'], though this means we cannot

  TRY(body_result, body, ParseBlock, env)

  const Location &rhs_loc = body->GetLocation();

  Location fn_loc = {lhs_loc.firstLine,
                     lhs_loc.firstColumn,
                     rhs_loc.lastLine,
                     rhs_loc.lastColumn};

  return {std::make_unique<Function>(fn_loc, name, args, std::move(body))};
}

/*
  arg = id ":" type
*/
auto Parser::ParseArgument(CompilationUnit &env)
    -> Outcome<std::pair<InternedString, Type::Pointer>, Error> {

  if (!Peek(Token::Id)) {
    return Error(Error::Code::MissingArgName, location, text);
  }

  const auto *name = env.InternVariable(text);

  nexttok(); // eat 'Id'

  if (!Expect(Token::Colon)) {
    Error(Error::Code::MissingArgColon, location, text);
  }

  if (Peek(Token::Comma) || Peek(Token::RParen)) {
    return Error(Error::Code::MissingArgType, location, text);
  }

  TRY(type_result, type, ParseType, env)

  return {std::make_pair(name, type)};
}

/*
  block = "{" {term} "}"
*/

auto Parser::ParseBlock(CompilationUnit &env) -> Parser::Result {
  std::vector<Ast::Pointer> expressions;
  Parser::Result            expression_result;
  Location                  left_loc = location;

  if (!Peek(Token::LBrace)) {
    return Error(Error::Code::MissingLBrace, location, text);
  }

  nexttok(); // eat '{'

  do {
    TRY(expression_result, expression, ParseTerm, env)

    expressions.emplace_back(std::move(expression));
  } while (!Peek(Token::RBrace));

  // the rhs location of the block is the right curly brace
  Location rhs_loc = location;

  nexttok(); // eat '}'

  Location block_loc(left_loc.firstLine,
                     left_loc.firstColumn,
                     rhs_loc.lastLine,
                     rhs_loc.lastColumn);
  return {std::make_unique<Block>(block_loc, std::move(expressions))};
}

/*
  term = conditional
       | while
       | bind
       | affix ";"
*/
auto Parser::ParseTerm(CompilationUnit &env) -> Parser::Result {
  if (Peek(Token::If)) {
    return ParseConditional(env);
  }

  if (Peek(Token::While)) {
    return ParseWhile(env);
  }

  if (Peek(Token::Var)) {
    return ParseBind(env);
  }

  TRY(affix_result, affix, ParseAffix, env)

  if (!Expect(Token::Semicolon)) {
    return {Error(Error::Code::MissingSemicolon, location, text)};
  }

  return {std::move(affix)};
}

/*
  conditional = "if" "(" affix ")" block "else" block
*/
auto Parser::ParseConditional(CompilationUnit &env) -> Parser::Result {
  Location lhs_loc = location;

  nexttok(); // eat "if"

  if (!Expect(Token::LParen)) {
    return Error(Error::Code::MissingLParen, location, text);
  }

  TRY(test_result, test_term, ParseAffix, env)

  if (!Expect(Token::RParen)) {
    return Error(Error::Code::MissingRParen, location, text);
  }

  TRY(then_result, then_term, ParseBlock, env)

  if (!Expect(Token::Else)) {
    return Error(Error::Code::MissingElse, location, text);
  }

  TRY(else_result, else_term, ParseBlock, env)

  const Location &rhs_loc = location;
  Location        condloc(lhs_loc.firstLine,
                   lhs_loc.firstColumn,
                   rhs_loc.lastLine,
                   rhs_loc.lastColumn);
  return {std::make_unique<IfThenElse>(condloc,
                                       std::move(test_term),
                                       std::move(then_term),
                                       std::move(else_term))};
}

/*
  while = "while" "(" affix ")" block
*/
auto Parser::ParseWhile(CompilationUnit &env) -> Parser::Result {
  Location lhs_loc = location;
  nexttok(); // eat 'while'

  TRY(test_result, test_term, ParseAffix, env)

  if (!Expect(Token::Do)) {
    return Error(Error::Code::MissingDo, location, text);
  }

  TRY(body_result, body_term, ParseBlock, env)

  const Location &rhs_loc = location;
  Location        whileloc(lhs_loc.firstLine,
                    lhs_loc.firstColumn,
                    rhs_loc.lastLine,
                    rhs_loc.lastColumn);
  return {std::make_unique<While>(whileloc,
                                  std::move(test_term),
                                  std::move(body_term))};
}

/*
  affix = composite "=" affix
        | composite "(" [affix {"," affix}] ")"
        | composite
*/
auto Parser::ParseAffix(CompilationUnit &env)
    -> Outcome<std::unique_ptr<Ast>, Error> {
  TRY(composite_result, composite, ParseComposite, env)

  // composite "=" affix
  if (Peek(Token::Assign)) {
    return ParseAssignment(std::move(composite), env);
  }

  // composite
  return {std::move(composite)};
}

/*
  composite '=' affix
*/
auto Parser::ParseAssignment(Ast::Pointer composite, CompilationUnit &env)
    -> Parser::Result {
  nexttok(); // eat "="

  TRY(right_result, right_term, ParseAffix, env)

  const Location &lhs_loc = composite->GetLocation();
  const Location &rhs_loc = right_term->GetLocation();
  Location        assign_loc(lhs_loc.firstLine,
                      lhs_loc.firstColumn,
                      rhs_loc.lastLine,
                      rhs_loc.lastColumn);
  return {std::make_unique<Assignment>(assign_loc,
                                       std::move(composite),
                                       std::move(right_term))};
}

/*
  callee "(" [affix {"," affix}] ")"
*/
auto Parser::ParseApplication(Ast::Pointer callee, CompilationUnit &env)
    -> Parser::Result {
  nexttok(); // eat '('

  std::vector<Ast::Pointer> args;

  if (!Peek(Token::RParen)) {
    // parse the argument list
    do {
      if (Peek(Token::Comma)) {
        nexttok();
      }

      TRY(arg_result, arg, ParseAffix, env)

      args.emplace_back(std::move(arg));

    } while (token == Token::Comma);

    // finish parsing the argument list
    if (!Peek(Token::RParen)) {
      return Error(Error::Code::MissingRParen, location, text);
    }
  }

  nexttok(); // eat ')'

  const Location &lhs_loc = callee->GetLocation();
  auto            rhs_loc = location;
  Location        app_loc(lhs_loc.firstLine,
                   lhs_loc.firstColumn,
                   rhs_loc.lastLine,
                   rhs_loc.lastColumn);
  return {std::make_unique<Application>(app_loc,
                                        std::move(callee),
                                        std::move(args))};
}

/*
  composite = accessor operator infix-parser
            | accessor
*/
auto Parser::ParseComposite(CompilationUnit &env) -> Parser::Result {
  TRY(accessor_result, accessor, ParseBuiltin, env)

  if (IsBinop(token)) {
    return ParseInfix(std::move(accessor), 0, env);
  }

  return {std::move(accessor)};
}

/*
  builtin = basic {"." basic | "[" basic "]" | "(" [affix {"," affix}] ")" }
           | basic
*/
auto Parser::ParseBuiltin(CompilationUnit &env) -> Parser::Result {
  TRY(left_result, left_term, ParseBasic, env)

  do {
    if (Peek(Token::Dot)) {
      left_result = ParseDot(std::move(left_term), env);
    } else if (Peek(Token::LBracket)) {
      left_result = ParseSubscript(std::move(left_term), env);
    } else if (Peek(Token::LParen)) {
      left_result = ParseApplication(std::move(left_term), env);
    } else {
      break;
    }
  } while (Peek(Token::Dot) || Peek(Token::LBracket) || Peek(Token::LParen));

  return left_result;
}

/*
 dot = basic "." basic
*/
auto Parser::ParseDot(Ast::Pointer left, CompilationUnit &env)
    -> Parser::Result {
  auto lhs_loc = left->GetLocation();
  nexttok(); // eat '.'

  TRY(right_result, right, ParseBasic, env)

  const Location &rhs_loc = right->GetLocation();
  Location        dotloc(lhs_loc.firstLine,
                  lhs_loc.firstColumn,
                  rhs_loc.lastLine,
                  rhs_loc.lastColumn);
  return {std::make_unique<Dot>(dotloc, std::move(left), std::move(right))};
}

/*
 subscript = basic "[" basic "]"
*/
auto Parser::ParseSubscript(Ast::Pointer left, CompilationUnit &env)
    -> Parser::Result {
  auto lhs_loc = left->GetLocation();
  nexttok(); // eat '['

  TRY(right_result, right, ParseBasic, env)

  if (!Expect(Token::RBracket)) { // eat ']'
    return {Error(Error::Code::MissingRBracket, location, text)};
  }

  auto     rhs_loc = location;
  Location location(lhs_loc.firstLine,
                    lhs_loc.firstColumn,
                    rhs_loc.lastLine,
                    rhs_loc.lastColumn);
  return {
      std::make_unique<Subscript>(location, std::move(left), std::move(right))};
}

/*
    This is an implementation of an operator precedence parser
    going from this pseudo-code:
    https://en.wikipedia.org/wiki/Operator-precedence_parser
*/
auto Parser::ParseInfix(Ast::Pointer     lhs,
                        pink::Precedence precedence,
                        CompilationUnit &env) -> Parser::Result {
  Parser::Result result{std::move(lhs)};
  Token          op{Token::Error};
  Location       op_loc{};

  auto PredictsBinop = [&]() -> bool {
    if (!IsBinop(token)) {
      return false;
    }

    return Precedence(token) >= precedence;
  };

  auto PredictsHigherPrecedenceOrRightAssociativeBinop = [&]() -> bool {
    if (!IsBinop(token)) {
      return false;
    }

    if (Precedence(token) > Precedence(op)) {
      return true;
    }

    if ((Associativity(op) == Associativity::Right) &&
        (Precedence(op) == Precedence(token))) {
      return true;
    }
    return false;
  };

  while (PredictsBinop()) {
    op     = token;
    op_loc = location;

    nexttok(); // eat the 'operator'

    TRY(right_result, right, ParseBuiltin, env)

    while (PredictsHigherPrecedenceOrRightAssociativeBinop()) {
      auto temp = ParseInfix(std::move(right), Precedence(op), env);
      if (!temp) {
        return temp.GetSecond();
      }
      right_result = std::move(temp);
    }

    const Location &rhs_loc = right_result.GetFirst()->GetLocation();
    Location        binop_loc(op_loc.firstLine,
                       op_loc.firstColumn,
                       rhs_loc.lastLine,
                       rhs_loc.lastColumn);
    result = std::make_unique<Binop>(binop_loc,
                                     op,
                                     std::move(result.GetFirst()),
                                     std::move(right));
  }

  return {std::move(result.GetFirst())};
}

/*
  basic = id (:= affix)?
        | "nil"
        | integer
        | operator basic
        | "true"
        | "false"
        | "(" affix {"," affix} ")"
        | "[" affix {"," affix} "]"

*/
auto Parser::ParseBasic(CompilationUnit &env) -> Parser::Result {
  switch (token) {
  case Token::Id: {
    Location       lhs_loc = location;
    InternedString symbol  = env.InternVariable(text);

    nexttok(); // eat the identifier

    if (Peek(Token::ColonEq)) {
      return ParseBind(symbol, lhs_loc, env);
    }

    return {std::make_unique<Variable>(lhs_loc, symbol)};
  }

  // #RULE: "!" and "-" are unary operators
  case Token::Not:
  case Token::Sub: {
    Location lhs_loc{location};
    Token    op = token; // NOLINT

    nexttok(); // eat op

    // #RULE unops all bind to their immediate right hand side
    TRY(right_result, right, ParseBuiltin, env)

    const Location &rhs_loc{right->GetLocation()};
    Location        unop_loc{lhs_loc.firstLine,
                      lhs_loc.firstColumn,
                      rhs_loc.lastLine,
                      rhs_loc.lastColumn};
    return {std::make_unique<Unop>(unop_loc, op, std::move(right))};
  }

  // #RULE '&' in basic position is the address of operator
  case Token::And: {
    Location lhs_loc{location};

    nexttok(); // eat '&'

    TRY(right_result, right, ParseBuiltin, env)

    const Location &rhs_loc{right->GetLocation()};
    Location        address_of_loc{lhs_loc.firstLine,
                            lhs_loc.firstColumn,
                            rhs_loc.lastLine,
                            rhs_loc.lastColumn};
    return {std::make_unique<AddressOf>(address_of_loc, std::move(right))};
  }

  // #RULE '*' in basic position is the address of operator
  case Token::Star: {
    Location lhs_loc{location};

    nexttok(); // eat '*'

    TRY(right_result, right, ParseBuiltin, env)

    const Location &rhs_loc{right->GetLocation()};
    Location        value_of_loc{lhs_loc.firstLine,
                          lhs_loc.firstColumn,
                          rhs_loc.lastLine,
                          rhs_loc.lastColumn};
    return {std::make_unique<ValueOf>(value_of_loc, std::move(right))};
  }

  // #RULE a open paren appearing in basic position
  // is either a parenthesized affix expression or
  // a tuple literal.
  case Token::LParen: // "("
  {
    nexttok(); // eat the '('

    TRY(expression_result, expression, ParseAffix, env)

    if (Peek(Token::Comma)) {
      return ParseTuple(std::move(expression), env);
    }
    // else we parsed a parenthised expression
    if (!Expect(Token::RParen)) {
      return {Error(Error::Code::MissingRParen, location, text)};
    }

    return {std::move(expression)};
  }

  // #RULE a bracket appearing in basic position is an array literal
  case Token::LBracket: {
    return ParseArray(env);
  }

  // #RULE Token::Nil at basic position is the literal nil
  case Token::Nil: {
    Location lhs_loc = location;
    nexttok(); // eat 'nil'
    return {std::make_unique<Nil>(lhs_loc)};
  }

  // #RULE Token::Int at basic position is a literal integer
  case Token::Integer: {
    Location lhs_loc       = location;
    auto     maybe_integer = ToNumber<Integer::Value>(text);
    if (!maybe_integer) {
      return Error{maybe_integer.GetSecond(), location, text};
    }

    nexttok(); // eat [0-9]+
    return {std::make_unique<Integer>(lhs_loc, maybe_integer.GetFirst())};
  }

  // #RULE Token::True at basic position is the literal true
  case Token::True: {
    Location lhs_loc = location;
    nexttok(); // Eat "true"
    return {std::make_unique<Boolean>(lhs_loc, true)};
  }

  // #RULE Token::False at basic position is the literal false
  case Token::False: {
    Location lhs_loc = location;
    nexttok(); // Eat "false"
    return {std::make_unique<Boolean>(lhs_loc, false)};
  }

  default: {
    return {Error(Error::Code::UnknownBasicToken, location, text)};
  }
  } // !switch(token)
}

auto Parser::ParseArray(CompilationUnit &env) -> Result {
  Location lhs_loc = location;
  nexttok(); // eat '['

  std::vector<Ast::Pointer> elements;

  do {
    if (Peek(Token::Comma)) {
      nexttok(); // eat ','
    }

    TRY(element_result, element, ParseAffix, env)

    elements.emplace_back(std::move(element));
  } while (token == Token::Comma);

  if (token != Token::RBracket) {
    return Error(Error::Code::MissingRBracket, location, text);
  }

  Location rhs_loc = location;
  nexttok(); // eat ']'

  Location array_loc(lhs_loc.firstLine,
                     lhs_loc.firstColumn,
                     rhs_loc.lastLine,
                     rhs_loc.lastColumn);
  return {std::make_unique<Array>(array_loc, std::move(elements))};
}

auto Parser::ParseTuple(Ast::Pointer first_element, CompilationUnit &env)
    -> Parser::Result {
  const Location           &lhs_loc = first_element->GetLocation();
  std::vector<Ast::Pointer> elements;
  elements.emplace_back(std::move(first_element));

  while (Expect(Token::Comma)) {
    TRY(element_result, element, ParseAffix, env)
    elements.emplace_back(std::move(element));
  }

  if (!Peek(Token::RParen)) {
    return Error(Error::Code::MissingRParen, location, text);
  }

  Location rhs_loc = location;

  nexttok(); // eat ')'

  Location tupleloc(lhs_loc.firstLine,
                    lhs_loc.firstColumn,
                    rhs_loc.lastLine,
                    rhs_loc.lastColumn);
  return {std::make_unique<Tuple>(tupleloc, std::move(elements))};
}

/*
  type = "Nil"
       | "Int"
       | "Bool"
       | "(" type {"," type} ")"
       | "[" type ";" int "]"
       | "*" type
       | "*[]" type
       | "fn" "(" type {"," type} ")" "->" type
*/
auto Parser::ParseType(CompilationUnit &env) -> Parser::TypeResult {
  switch (token) {
  // #RULE Token::NilType is the type Nil
  case Token::NilType: {
    nexttok(); // eat 'Nil'
    return {env.GetNilType()};
    break;
  }

  // #RULE Token::IntegerType is the type Int
  case Token::IntegerType: {
    nexttok(); // Eat "Int"
    return {env.GetIntType()};
    break;
  }

  // #RULE Token::BooleanType is the type Bool
  case Token::BooleanType: {
    nexttok(); // Eat "Bool"
    return {env.GetBoolType()};
    break;
  }

  // #RULE Token::LParen predicts the type Tuple
  case Token::LParen: {
    return ParseTupleType(env);
  }

  // #RULE Token::LBracket predicts the type Array
  case Token::LBracket: {
    return ParseArrayType(env);
  }

  // #RULE Token::Star predicts the type of a pointer
  case Token::Star: {
    return ParsePointerType(env);
  }

  // #RULE Token::Fn predicts the type of a function
  case Token::Fn: {
    return ParseFunctionType(env);
  }

  default: {
    return {Error(Error::Code::UnknownTypeToken, location, text)};
  }
  }
}

auto Parser::ParseTupleType(CompilationUnit &env) -> TypeResult {
  nexttok(); // eat '('
  TRY(left_result, left, ParseType, env)

  if (token == Token::Comma) {
    TupleType::Elements elements;
    elements.push_back(left);
    do {
      nexttok(); // eat ','

      TRY(element_result, element, ParseType, env)
      elements.push_back(element);
    } while (token == Token::Comma);

    left = env.GetTupleType(std::move(elements));
  }

  if (!Expect(Token::RParen)) {
    return Error(Error::Code::MissingRParen, location, text);
  }

  return left;
}

auto Parser::ParseArrayType(CompilationUnit &env) -> TypeResult {
  nexttok(); // eat '['

  TRY(array_type_result, array_type, ParseType, env)

  if (!Expect(Token::Semicolon)) {
    return {Error(Error::Code::MissingArraySemicolon, location, text)};
  }

  if (!Peek(Token::Integer)) {
    return {Error(Error::Code::MissingArrayNum, location, text)};
  }

  auto maybe_integer = ToNumber<Integer::Value>(text);
  if (!maybe_integer) {
    return Error{maybe_integer.GetSecond(), location, text};
  }

  nexttok(); // eat [0-9]+

  if (!Expect(Token::RBracket)) {
    return {Error(Error::Code::MissingRBracket, location, text)};
  }

  return {env.GetArrayType(maybe_integer.GetFirst(), array_type)};
}

auto Parser::ParsePointerType(CompilationUnit &env) -> TypeResult {
  nexttok(); // eat "*"

  if (Expect(Token::LBracket)) {
    if (!Expect(Token::RBracket)) {
      return {Error{Error::Code::MissingRBrace, location, text}};
    }
    TRY(type_result, type, ParseType, env)
    return {env.GetSliceType(type)};
  }

  TRY(type_result, type, ParseType, env)
  return {env.GetPointerType(type)};
}

auto Parser::ParseFunctionType(CompilationUnit &env) -> TypeResult {
  nexttok(); // eat "fn"

  if (!Expect(Token::LParen)) {
    return {Error{Error::Code::MissingLParen, location, text}};
  }

  FunctionType::Arguments arguments;
  // if we see ")", then we skip parsing the argument list
  // because there are none.
  // if we don't see ")" then we attempt to parse the argument list
  if (!Expect(Token::RParen)) {
    do {
      if (Peek(Token::Comma)) {
        nexttok();
      }

      TRY(argument_result, argument, ParseType, env)
      arguments.push_back(argument);

    } while (Peek(Token::Comma));

    // #RULE the argument list must end with ")"
    if (!Expect(Token::RParen)) {
      return {Error{Error::Code::MissingRParen, location, text}};
    }
  }

  if (!Expect(Token::RArrow)) {
    return {Error{Error::Code::MissingRArrow, location, text}};
  }

  TRY(result_result, result, ParseType, env)
  return {env.GetFunctionType(result, std::move(arguments))};
}

} // namespace pink

#undef TRY