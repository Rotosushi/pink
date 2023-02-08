#include "front/Parser.h"

#include <charconv> // std::from_chars

#include "aux/Environment.h"

// Syntactic Forms
#include "ast/Application.h"
#include "ast/Array.h"
#include "ast/Assignment.h"
#include "ast/Bind.h"
#include "ast/Block.h"
#include "ast/Conditional.h"
#include "ast/Dot.h"
#include "ast/Function.h"
#include "ast/Subscript.h"
#include "ast/Tuple.h"
#include "ast/Variable.h"
#include "ast/While.h"

// Values
#include "ast/Boolean.h"
#include "ast/Integer.h"
#include "ast/Nil.h"

// Operators
#include "ast/Binop.h"
#include "ast/Unop.h"

// Types
#include "type/ArrayType.h"
#include "type/BoolType.h"
#include "type/IntType.h"
#include "type/NilType.h"
#include "type/PointerType.h"
#include "type/TupleType.h"

// We cannot replace this with a template function,
// as we need the return statement to be in the scope
// this snippet of code is inserted into.
#define TRY(result, variable, function, ...)                                   \
  auto result = function(__VA_ARGS__);                                         \
  if (!result) {                                                               \
    return result;                                                             \
  }                                                                            \
  auto &variable = result.GetFirst();

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
  token = lexer.yylex(); // this statement advances the lexer's internal state.
  location = lexer.yyloc();
  text     = lexer.yytxt();

  while (lexer.EndOfInput() && !input_stream->eof()) {
    auto line = Getline();
    lexer.AppendToBuffer(line);
    token    = lexer.yylex();
    location = lexer.yyloc();
    text     = lexer.yytxt();
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

/*
  Extract the n'th line of source text from the buffer,
  where n = location.firstLine
*/
auto Parser::ExtractLine(const Location &location) const -> std::string_view {
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

static auto ToInteger(std::string_view text)
    -> Outcome<Integer::Value, std::errc> {
  Integer::Value value{};
  auto [ptr,
        errc]{std::from_chars<Integer::Value>(text.begin(), text.end(), value)};
  if (errc == std::errc()) {
    return {value};
  }
  return {errc};
}

auto Parser::Parse(Environment &env) -> Parser::Result {
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
auto Parser::ParseTop(Environment &env) -> Parser::Result {
  if (Peek(Token::Fn)) {
    return ParseFunction(env);
  }

  if (Peek(Token::Id) || Peek(Token::Var)) {
    auto outcome = ParseBind(env);
    if (!outcome) {
      // So, we cannot simply [return outcome]
      // because the return statement selects
      // the copy constructor of
      return outcome;
    }

    if (!Expect(Token::Semicolon)) {
      return {Error(Error::MissingSemicolon, location, std::string(text))};
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
auto Parser::ParseBind(Environment &env) -> Parser::Result {
  Location lhs_loc = location;

  if (Peek(Token::Var)) {
    nexttok(); // eat 'var'
  }

  if (!Peek(Token::Id)) {
    return Error(Error::Code::MissingBindId, location, std::string(text));
  }

  const auto *name = env.variable_interner.Intern(text);

  nexttok(); // eat id

  return ParseBind(name, lhs_loc, env);
}

auto Parser::ParseBind(InternedString name,
                       Location       lhs_location,
                       Environment   &env)
    -> Outcome<std::unique_ptr<Ast>, Error> {
  if (!Expect(Token::ColonEq)) {
    return Error(Error::Code::MissingBindColonEq, location, std::string(text));
  }

  auto affix = ParseAffix(env);
  if (!affix) {
    return {affix.GetSecond()};
  }

  Location bind_loc(lhs_location.firstLine,
                    lhs_location.firstColumn,
                    location.lastLine,
                    location.lastColumn);
  return {std::make_unique<Bind>(bind_loc, name, std::move(affix.GetFirst()))};
}

/*
  function = "fn" id "(" [arg {"," arg}] ")" block
*/
auto Parser::ParseFunction(Environment &env) -> Parser::Result {
  InternedString      name = nullptr;
  Function::Arguments args;
  Ast::Pointer        body(nullptr);
  Location            lhs_loc = location;

  nexttok(); // eat 'fn'

  if (!Peek(Token::Id)) {
    return Error(Error::Code::MissingFnName, location, std::string(text));
  }

  name = env.variable_interner.Intern(text); // intern the functions name

  nexttok(); // eat 'id'

  if (!Expect(Token::LParen)) {
    return Error(Error::Code::MissingLParen, location, std::string(text));
  }

  // #RULE a functions argument list is optional
  if (Peek(Token::Id)) {
    do {
      auto arg = ParseArgument(env);

      if (!arg) {
        return {arg.GetSecond()};
      }

      args.emplace_back(std::move(arg.GetFirst()));
    } while (Expect(Token::Comma));
  }

  if (!Expect(Token::RParen)) {
    return Error(Error::Code::MissingRParen, location, std::string(text));
  }

  // #PROPOSAL we could easily parse functions which have no
  // block expression if we allow the alternative
  // to be affix ';'

  // parse the body of the function.
  auto body_res = ParseBlock(env);
  if (!body_res) {
    return body_res.GetSecond();
  }

  const Location &rhs_loc = body_res.GetFirst()->GetLocation();

  Location fn_loc = {lhs_loc.firstLine,
                     lhs_loc.firstColumn,
                     rhs_loc.lastLine,
                     rhs_loc.lastColumn};

  return {std::make_unique<Function>(fn_loc,
                                     name,
                                     args,
                                     std::move(body_res.GetFirst()))};
}

/*
  arg = id ":" type
*/
auto Parser::ParseArgument(Environment &env)
    -> Outcome<std::pair<InternedString, Type::Pointer>, Error> {

  if (!Peek(Token::Id)) {
    return Error(Error::Code::MissingArgName, location, std::string(text));
  }

  const auto *name = env.variable_interner.Intern(text);

  nexttok(); // eat 'Id'

  if (!Expect(Token::Colon)) {
    Error(Error::Code::MissingArgColon, location, std::string(text));
  }

  if (Peek(Token::Comma) || Peek(Token::RParen)) {
    return Error(Error::Code::MissingArgType, location, std::string(text));
  }

  auto type_result = ParseType(env);
  if (!type_result) {
    return type_result.GetSecond();
  }

  return {std::make_pair(name, type_result.GetFirst())};
}

/*
  block = "{" {term} "}"
*/

auto Parser::ParseBlock(Environment &env) -> Parser::Result {
  std::vector<Ast::Pointer> expressions;
  Parser::Result            expression_result;
  Location                  left_loc = location;

  if (!Peek(Token::LBrace)) {
    return Error(Error::Code::MissingLBrace, location, std::string(text));
  }

  nexttok(); // eat '{'

  do {
    expression_result = ParseTerm(env);
    if (!expression_result) {
      return expression_result.GetSecond();
    }
    auto &expression = expression_result.GetFirst();

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
auto Parser::ParseTerm(Environment &env) -> Parser::Result {
  if (Peek(Token::If)) {
    return ParseConditional(env);
  }

  if (Peek(Token::While)) {
    return ParseWhile(env);
  }

  if (Peek(Token::Var)) {
    return ParseBind(env);
  }

  // else assume this is an affix term
  auto affix_result = ParseAffix(env);
  if (!affix_result) {
    return affix_result.GetSecond();
  }
  auto &affix = affix_result.GetFirst();

  if (!Expect(Token::Semicolon)) {
    return {Error(Error::Code::MissingSemicolon, location, std::string(text))};
  }

  return {std::move(affix)};
}

/*
  conditional = "if" "(" affix ")" block "else" block
*/
auto Parser::ParseConditional(Environment &env) -> Parser::Result {
  Location lhs_loc = location;

  nexttok(); // eat "if"

  if (!Expect(Token::LParen)) {
    return Error(Error::Code::MissingLParen, location, std::string(text));
  }

  auto test_term = ParseAffix(env);
  if (!test_term) {
    return {test_term.GetSecond()};
  }
  auto &test = test_term.GetFirst();

  if (!Expect(Token::RParen)) {
    return Error(Error::Code::MissingRParen, location, std::string(text));
  }

  auto then_term = ParseBlock(env);
  if (!then_term) {
    return {then_term.GetSecond()};
  }
  auto &then = then_term.GetFirst();

  if (!Expect(Token::Else)) {
    return Error(Error::Code::MissingElse, location, std::string(text));
  }

  auto else_term = ParseBlock(env);
  if (!else_term) {
    return {else_term.GetSecond()};
  }
  auto &els = else_term.GetFirst();

  const Location &rhs_loc = els->GetLocation();
  Location        condloc(lhs_loc.firstLine,
                   lhs_loc.firstColumn,
                   rhs_loc.lastLine,
                   rhs_loc.lastColumn);
  return {std::make_unique<Conditional>(condloc,
                                        std::move(test),
                                        std::move(then),
                                        std::move(els))};
}

/*
  while = "while" "(" affix ")" block
*/
auto Parser::ParseWhile(Environment &env) -> Parser::Result {
  Location lhs_loc = location;
  nexttok(); // eat 'while'

  auto test_result = ParseAffix(env);
  if (!test_result) {
    return {test_result.GetSecond()};
  }
  auto &test = test_result.GetFirst();

  if (!Expect(Token::Do)) {
    return Error(Error::Code::MissingDo, location, std::string(text));
  }

  auto body_term = ParseBlock(env);
  if (!body_term) {
    return body_term.GetSecond();
  }
  auto &body = body_term.GetFirst();

  const Location &rhs_loc = body_term.GetFirst()->GetLocation();
  Location        whileloc(lhs_loc.firstLine,
                    lhs_loc.firstColumn,
                    rhs_loc.lastLine,
                    rhs_loc.lastColumn);
  return {std::make_unique<While>(whileloc, std::move(test), std::move(body))};
}

/*
  affix = composite "=" affix
        | composite "(" [affix {"," affix}] ")"
        | composite
*/
auto Parser::ParseAffix(Environment &env)
    -> Outcome<std::unique_ptr<Ast>, Error> {
  auto composite_term = ParseComposite(env);
  if (!composite_term) {
    return {composite_term.GetSecond()};
  }
  auto &composite = composite_term.GetFirst();

  // composite "=" affix
  if (Peek(Token::Equals)) {
    return ParseAssignment(std::move(composite), env);
  }

  // composite "(" [affix {"," affix}] ")"
  if (Peek(Token::LParen)) {
    return ParseApplication(std::move(composite), env);
  }

  // composite
  return {std::move(composite)};
}

/*
  composite '=' affix
*/
auto Parser::ParseAssignment(Ast::Pointer composite, Environment &env)
    -> Parser::Result {
  nexttok(); // eat "="

  auto rhs_term = ParseAffix(env);
  if (!rhs_term) {
    return rhs_term.GetSecond();
  }
  auto &rhs = rhs_term.GetFirst();

  const Location &lhs_loc = composite->GetLocation();
  const Location &rhs_loc = rhs_term.GetFirst()->GetLocation();
  Location        assign_loc(lhs_loc.firstLine,
                      lhs_loc.firstColumn,
                      rhs_loc.lastLine,
                      rhs_loc.lastColumn);
  return {std::make_unique<Assignment>(assign_loc,
                                       std::move(composite),
                                       std::move(rhs))};
}

/*
  composite "(" [affix {"," affix}] ")"
*/
auto Parser::ParseApplication(Ast::Pointer composite, Environment &env)
    -> Parser::Result {
  Location rhs_loc;
  nexttok(); // eat '('

  std::vector<Ast::Pointer> args;

  if (!Peek(Token::RParen)) {
    // parse the argument list
    do {
      if (Peek(Token::Comma)) {
        nexttok();
      }

      auto arg = ParseAffix(env);
      if (!arg) {
        return arg.GetSecond();
      }

      args.emplace_back(std::move(arg.GetFirst()));

    } while (token == Token::Comma);

    // finish parsing the argument list
    if (!Peek(Token::RParen)) {
      return Error(Error::Code::MissingRParen, location, std::string(text));
    }

    rhs_loc = location;

    nexttok(); // eat ')'
  } else {     // token == Token::RParen

    rhs_loc = location;

    nexttok(); // eat ')'
    // the argument list is empty, but this is still an application term.
  }

  const Location &lhs_loc = composite->GetLocation();
  Location        app_loc(lhs_loc.firstLine,
                   lhs_loc.firstColumn,
                   rhs_loc.lastLine,
                   rhs_loc.lastColumn);
  return {std::make_unique<Application>(app_loc,
                                        std::move(composite),
                                        std::move(args))};
}

/*
  composite = accessor operator infix-parser
            | accessor
*/
auto Parser::ParseComposite(Environment &env) -> Parser::Result {
  auto accessor_term = ParseAccessor(env);
  if (!accessor_term) {
    return accessor_term.GetSecond();
  }
  auto &accessor = accessor_term.GetFirst();

  if (Peek(Token::Op)) {
    // this is actually a binop expression
    // where the accessor term is the first
    // left hand side term.
    return ParseInfix(std::move(accessor), 0, env);
  }
  return {std::move(accessor)};
}

/*
  accessor = basic { ("." basic | "[" basic "]") }
           | basic
*/
auto Parser::ParseAccessor(Environment &env) -> Parser::Result {
  auto left_term = ParseBasic(env);
  if (!left_term) {
    return left_term.GetSecond();
  }
  auto &left = left_term.GetFirst();

  do {
    if (Peek(Token::Dot)) {
      left_term = ParseDot(std::move(left), env);
    } else if (Peek(Token::LBracket)) {
      left_term = ParseSubscript(std::move(left), env);
    }

  } while (Peek(Token::Dot) || Peek(Token::LBracket));

  return {std::move(left)};
}

/*
 dot = basic "." basic
*/
auto Parser::ParseDot(Ast::Pointer left, Environment &env) -> Parser::Result {
  auto lhs_loc = left->GetLocation();
  nexttok(); // eat '.'

  auto right_term = ParseBasic(env);
  if (!right_term) {
    return {right_term.GetSecond()};
  }
  auto &right = right_term.GetFirst();

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
auto Parser::ParseSubscript(Ast::Pointer left, Environment &env)
    -> Parser::Result {
  auto lhs_loc = left->GetLocation();
  nexttok(); // eat '['

  auto right_term = ParseBasic(env);
  if (!right_term) {
    return {right_term.GetSecond()};
  }
  auto &right = right_term.GetFirst();

  if (!Expect(Token::RBracket)) { // eat ']'
    return {Error(Error::Code::MissingRBracket, location, std::string(text))};
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
auto Parser::ParseInfix(Ast::Pointer lhs,
                        Precedence   precedence,
                        Environment &env) -> Parser::Result {
  Parser::Result result(std::move(lhs));
  InternedString peek_str = nullptr;
  std::optional<std::pair<InternedString, BinopLiteral *>> peek_opt;
  BinopLiteral                                            *peek_lit = nullptr;
  InternedString                                           op_str   = nullptr;
  BinopLiteral                                            *op_lit   = nullptr;
  Location                                                 op_loc;

  auto PredictsBinop = [&]() -> bool {
    return (token == Token::Op) &&
           ((peek_str = env.operator_interner.Intern(text)) != nullptr) &&
           (peek_opt = env.binop_table.Lookup(peek_str)) &&
           (peek_opt.has_value()) &&
           ((peek_lit = peek_opt->second) != nullptr) &&
           (peek_lit->GetPrecedence() >= precedence);
  };

  auto PredictsHigherPrecedenceOrRightAssociativeBinop = [&]() -> bool {
    return (token == Token::Op) &&
           ((peek_str = env.operator_interner.Intern(text)) != nullptr) &&
           (peek_opt = env.binop_table.Lookup(peek_str)) &&
           (peek_opt.has_value()) &&
           ((peek_lit = peek_opt->second) != nullptr) &&
           ((peek_lit->GetPrecedence() > op_lit->GetPrecedence()) ||
            ((peek_lit->GetAssociativity() == Associativity::Right) &&
             (peek_lit->GetPrecedence() == op_lit->GetPrecedence())));
  };

  while (PredictsBinop()) {
    op_str = peek_str;
    op_lit = peek_lit;
    op_loc = location;

    nexttok(); // eat the 'operator'

    Parser::Result rhs = ParseAccessor(env);
    if (!rhs) {
      return rhs.GetSecond();
    }

    while (PredictsHigherPrecedenceOrRightAssociativeBinop()) {
      auto temp =
          ParseInfix(std::move(rhs.GetFirst()), peek_lit->GetPrecedence(), env);
      if (!temp) {
        return temp.GetSecond();
      }
      rhs = std::move(temp.GetFirst());
    }

    const Location &rhs_loc = rhs.GetFirst()->GetLocation();
    Location        binop_loc(op_loc.firstLine,
                       op_loc.firstColumn,
                       rhs_loc.lastLine,
                       rhs_loc.lastColumn);
    result =
        Parser::Result(std::make_unique<Binop>(binop_loc,
                                               op_str,
                                               std::move(result.GetFirst()),
                                               std::move(rhs.GetFirst())));
  }

  return {std::move(result.GetFirst())};
}

/*
  basic = id (:= affix)?
        | integer
        | operator basic
        | "true"
        | "false"
        | "(" affix {"," affix} ")"
        | "[" affix {"," affix} "]"

*/
auto Parser::ParseBasic(Environment &env) -> Parser::Result {
  switch (token) {
  case Token::Id: {
    Location       lhs_loc = location;
    InternedString symbol  = env.variable_interner.Intern(text);

    nexttok(); // eat the identifier

    if (Peek(Token::ColonEq)) {
      return ParseBind(symbol, lhs_loc, env);
    }

    return {std::make_unique<Variable>(lhs_loc, symbol)};
  }

  // #RULE: an operator appearing in the basic position is a unop
  case Token::Op: {
    Location       lhs_loc{location};
    InternedString opr{env.operator_interner.Intern(text)};

    nexttok(); // eat op

    // #RULE unops all bind to their immediate right hand side

    // we don't want to recurr to affix, as then a unop
    // will break any infix expression it appears within
    // in two, however we do want to easily take the
    // address of a member or indirect a pointer member,
    // so we also cannot recur to basic, thus, one step
    // below infix; accessor.
    auto rhs = ParseAccessor(env);
    if (!rhs) {
      return rhs.GetSecond();
    }

    const Location &rhs_loc{rhs.GetFirst()->GetLocation()};
    Location        unop_loc{lhs_loc.firstLine,
                      lhs_loc.firstColumn,
                      rhs_loc.lastLine,
                      rhs_loc.lastColumn};
    return {std::make_unique<Unop>(unop_loc, opr, std::move(rhs.GetFirst()))};
  }

  // #RULE a open paren appearing in basic position
  // is either a parenthesized affix expression or
  // a tuple literal.
  case Token::LParen: // "("
  {
    nexttok(); // eat the '('

    // wrap this sequence of steps into a MACRO
    auto expression_result = ParseAffix(env);
    if (!expression_result) {
      return expression_result.GetSecond();
    }
    auto &expression = expression_result.GetFirst();

    if (Peek(Token::Comma)) {
      return ParseTuple(std::move(expression), env);
    }
    // else we parsed a parenthised expression
    if (!Expect(Token::RParen)) {
      return {Error(Error::Code::MissingRParen, location, std::string(text))};
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
    auto     maybe_integer = ToInteger(text);
    if (maybe_integer) {
      nexttok(); // eat [0-9]+
      return {std::make_unique<Integer>(lhs_loc, maybe_integer.GetFirst())};
    }

    auto errc = maybe_integer.GetSecond();
    if (errc == std::errc::result_out_of_range) {
      return {
          Error(Error::Code::IntegerOutOfBounds, location, std::string(text))};
    }

    if (errc == std::errc::invalid_argument) {
      FatalError("Lexed [0-9]+ that std::from_chars doesn't recognize",
                 __FILE__,
                 __LINE__);
    }

    FatalError("Unhandled std::errc returned by from_chars",
               __FILE__,
               __LINE__);
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
    return {Error(Error::Code::UnknownBasicToken, location, std::string(text))};
  }
  } // !switch(token)
}

auto Parser::ParseArray(Environment &env) -> Result {
  Location lhs_loc = location;
  nexttok(); // eat '['

  std::vector<Ast::Pointer> members;

  do {
    if (Peek(Token::Comma)) {
      nexttok(); // eat ','
    }

    auto member = ParseAffix(env);
    if (!member) {
      return member.GetSecond();
    }

    members.emplace_back(std::move(member.GetFirst()));
  } while (token == Token::Comma);

  if (token != Token::RBracket) {
    return Error(Error::Code::MissingRBracket, location, std::string(text));
  }

  Location rhs_loc = location;
  nexttok(); // eat ']'

  Location array_loc(lhs_loc.firstLine,
                     lhs_loc.firstColumn,
                     rhs_loc.lastLine,
                     rhs_loc.lastColumn);
  return {std::make_unique<Array>(array_loc, std::move(members))};
}

auto Parser::ParseTuple(Ast::Pointer first_element, Environment &env)
    -> Parser::Result {
  const Location           &lhs_loc = first_element->GetLocation();
  Parser::Result            result;
  std::vector<Ast::Pointer> elements;
  elements.emplace_back(std::move(first_element));

  while (Expect(Token::Comma)) {
    result = Parser::Result(ParseAffix(env));

    if (!result) {
      return result.GetSecond();
    }

    elements.emplace_back(std::move(result.GetFirst()));
  }

  if (!Peek(Token::RParen)) {
    return Error(Error::Code::MissingRParen, location, std::string(text));
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
       | "Pointer" type
*/
auto Parser::ParseType(Environment &env) -> Parser::TypeResult {
  switch (token) {
  // #RULE Token::NilType is the type Nil
  case Token::NilType: {
    nexttok(); // eat 'Nil'
    return {env.type_interner.GetNilType()};
    break;
  }

  // #RULE Token::IntegerType is the type Int
  case Token::IntegerType: {
    nexttok(); // Eat "Int"
    return {env.type_interner.GetIntType()};
    break;
  }

  // #RULE Token::BooleanType is the type Bool
  case Token::BooleanType: {
    nexttok(); // Eat "Bool"
    return {env.type_interner.GetBoolType()};
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

  // #RULE Token::Pointer predicts the type of a pointer
  case Token::Pointer: {
    return ParsePointerType(env);
  }

  default: {
    return {Error(Error::Code::UnknownTypeToken, location, std::string(text))};
  }
  }
}

auto Parser::ParseTupleType(Environment &env) -> TypeResult {
  nexttok(); // eat '('
  auto left = ParseType(env);

  if (!left) {
    return left;
  }

  if (token == Token::Comma) {
    std::vector<Type::Pointer> elem_tys = {left.GetFirst()};
    do {
      nexttok(); // eat ','

      auto elem_ty = ParseType(env);
      if (!elem_ty) {
        return elem_ty;
      }

      elem_tys.push_back(elem_ty.GetFirst());
    } while (token == Token::Comma);

    left = env.type_interner.GetTupleType(elem_tys);
  }

  if (!Expect(Token::RParen)) {
    return Error(Error::Code::MissingRParen, location, std::string(text));
  }

  return left;
}

auto Parser::ParseArrayType(Environment &env) -> TypeResult {
  nexttok(); // eat '['

  auto array_type_result = ParseType(env);

  if (!array_type_result) {
    return array_type_result;
  }

  auto &array_type = array_type_result.GetFirst();

  if (!Expect(Token::Semicolon)) {
    return {
        Error(Error::Code::MissingArraySemicolon, location, std::string(text))};
  }

  if (!Peek(Token::Integer)) {
    return {Error(Error::Code::MissingArrayNum, location, std::string(text))};
  }

  auto maybe_integer = ToInteger(text);
  if (!maybe_integer) {
    auto errc = maybe_integer.GetSecond();
    if (errc == std::errc::result_out_of_range) {
      return {
          Error(Error::Code::IntegerOutOfBounds, location, std::string(text))};
    }

    if (errc == std::errc::invalid_argument) {
      FatalError("Lexed [0-9]+ that std::from_chars doesn't recognize",
                 __FILE__,
                 __LINE__);
    }

    FatalError("Unhandled std::errc returned by from_chars",
               __FILE__,
               __LINE__);
  }

  nexttok(); // eat [0-9]+

  if (!Expect(Token::RBracket)) {
    return {Error(Error::Code::MissingRBracket, location, std::string(text))};
  }

  return {env.type_interner.GetArrayType(maybe_integer.GetFirst(), array_type)};
}

auto Parser::ParsePointerType(Environment &env) -> TypeResult {
  nexttok(); // eat "Pointer"

  auto type = ParseType(env);

  if (!type) {
    return type;
  }

  return {env.type_interner.GetPointerType(type.GetFirst())};
}

} // namespace pink
