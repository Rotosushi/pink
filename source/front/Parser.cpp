#include "front/Parser.h"

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

namespace pink {
Parser::Parser() : tok(Token::End), loc(1, 0, 1, 0), input_stream(&std::cin) {}

Parser::Parser(std::istream *input_stream)
    : tok(Token::End), loc(1, 0, 1, 0), input_stream(input_stream) {
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

void Parser::yyfill() { lexer.Getline(*input_stream); }

void Parser::nexttok() {
  tok = lexer.yylex(); // this statement advances the lexer's internal state.
  loc = lexer.yyloc();
  txt = lexer.yytxt();

  while (lexer.EndOfInput() && !input_stream->eof()) {
    yyfill();
    tok = lexer.yylex();
    loc = lexer.yyloc();
    txt = lexer.yytxt();
  }
}

auto Parser::Expect(Token token) -> bool {
  if (tok == token) {
    nexttok();
    return true;
  }

  return false;
}

auto Parser::Peek(Token token) -> bool { return tok == token; }

/*
  Extract the n'th line of source text from the buffer,
  where n = loc.firstLine
*/
auto Parser::ExtractLine(const Location &loc) const -> std::string {
  const auto &buf = lexer.GetBuf();
  auto cursor = buf.begin();
  auto end = buf.end();

  size_t lines_seen = 1;

  while ((lines_seen < loc.firstLine) && (cursor != end)) {
    if (*cursor == '\n') {
      lines_seen++;
    }

    cursor++;
  }

  if (cursor != end) {
    auto eol = cursor;

    while (eol != end && *eol != '\n') {
      eol++;
    }

    // #NOTE eol points to end or to a newline char
    // either way the constructor we are using
    // uses to range [first, last), so we will
    // not copy the newline into the new string.
    return {cursor, eol};
  }
  return {};
}

auto Parser::Parse(const Environment &env)
    -> Outcome<std::unique_ptr<Ast>, Error> {
  // prime the lexer with the first token from the input stream;
  // only if we are not already parsing an input stream.
  if (Peek(Token::End) && lexer.EndOfInput() && !input_stream->eof()) {
    nexttok();
  }

  // if there is no more source return EndOfFile.
  if (Peek(Token::End) && input_stream->eof()) {
    return {Error(Error::Code::EndOfFile, loc)};
  }

  return ParseTop(env);
}

/*
  top = function
      | bind ';'
*/
auto Parser::ParseTop(const Environment &env)
    -> Outcome<std::unique_ptr<Ast>, Error> {
  if (Peek(Token::Fn)) {
    return ParseFunction(env);
  }

  if (Peek(Token::Id) || Peek(Token::Var)) {
    auto outcome = ParseBind(env);
    if (!outcome) {
      return {outcome.GetSecond()};
    }

    if (!Expect(Token::Semicolon)) {
      std::string errtxt = "instead saw: " + txt;
      return {Error(Error::MissingSemicolon, loc, errtxt)};
    }

    return {std::move(outcome.GetFirst())};
  }
  /// \note  is it worth it to attempt to parse the bad expression
  /// such that we don't leave the parser at the error we encountered?
  /// as it stands now we can only handle a single parse error before
  /// we bail. if we wanted to display more than the first error
  /// we encountered, we would need to do something along these lines.
  /// otherwise when we reentered the parser we would parse the exact
  /// same error.
  return {
      Error(Error::Code::BadTopLevelExpression, loc,
            "only variables and functions may be declared at global scope")};
}

/*
  bind = ("var")? id ":=" affix ";"
*/
auto Parser::ParseBind(const Environment &env)
    -> Outcome<std::unique_ptr<Ast>, Error> {
  Location lhs_loc = loc;

  if (Peek(Token::Var)) {
    nexttok(); // eat 'var'
  }

  if (!Peek(Token::Id)) {
    return Error(Error::Code::MissingBindId, loc, "parsed: " + txt);
  }

  const auto *name = env.symbols->Intern(txt);

  nexttok(); // eat id

  return ParseBind(name, lhs_loc, env);
}

auto Parser::ParseBind(InternedString name, Location lhs_location,
                       const Environment &env)
    -> Outcome<std::unique_ptr<Ast>, Error> {
  if (!Expect(Token::ColonEq)) {
    return Error(Error::Code::MissingBindColonEq, loc, "parsed: " + txt);
  }

  auto affix = ParseAffix(env);
  if (!affix) {
    return {affix.GetSecond()};
  }

  Location bind_loc(lhs_location.firstLine, lhs_location.firstColumn,
                    loc.lastLine, loc.lastColumn);
  return {std::make_unique<Bind>(bind_loc, name, std::move(affix.GetFirst()))};
}

/*
  function = "fn" id "(" [arg {"," arg}] ")" block
*/
auto Parser::ParseFunction(const Environment &env)
    -> Outcome<std::unique_ptr<Ast>, Error> {
  InternedString name = nullptr;
  std::vector<std::pair<InternedString, Type *>> args;
  std::unique_ptr<Ast> body(nullptr);
  Location lhs_loc = loc;

  nexttok(); // eat 'fn'

  if (!Peek(Token::Id)) {
    return Error(Error::Code::MissingFnName, loc);
  }

  name = env.symbols->Intern(txt); // intern the functions name

  nexttok(); // eat 'id'

  if (!Expect(Token::LParen)) {
    return Error(Error::Code::MissingLParen, loc);
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
    return Error(Error::Code::MissingRParen, loc);
  }

  // #PROPOSAL we could easily parse functions which have no
  // block expression if we allow the alternative
  // to be affix ';'
  if (!Peek(Token::LBrace)) {
    return Error(Error::Code::MissingLBrace, loc);
  }

  // parse the body of the function.
  auto body_res = ParseBlock(env);
  if (!body_res) {
    return body_res.GetSecond();
  }

  const Location &rhs_loc = body_res.GetFirst()->GetLoc();

  Location fn_loc = {lhs_loc.firstLine, lhs_loc.firstColumn, rhs_loc.lastLine,
                     rhs_loc.lastColumn};

  // we have all the parts, build the function.
  return {std::make_unique<Function>(
      fn_loc, name, args, std::move(body_res.GetFirst()), env.bindings.get())};
}

/*
  arg = id ":" type
*/
auto Parser::ParseArgument(const Environment &env)
    -> Outcome<std::pair<InternedString, Type *>, Error> {

  if (!Peek(Token::Id)) {
    return Error(Error::Code::MissingArgName, loc);
  }

  const auto *name = env.symbols->Intern(txt);

  nexttok(); // eat 'Id'

  if (!Expect(Token::Colon)) {
    Error(Error::Code::MissingArgColon, loc);
  }

  if (Peek(Token::Comma) || Peek(Token::RParen)) {
    return Error(Error::Code::MissingArgType, loc);
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

auto Parser::ParseBlock(const Environment &env)
    -> Outcome<std::unique_ptr<Ast>, Error> {
  std::vector<std::unique_ptr<Ast>> stmnts;
  Outcome<std::unique_ptr<Ast>, Error> result = Error();
  Location left_loc = loc;

  nexttok(); // eat '{'

  do {
    result = ParseTerm(env);
    if (!result) {
      return result.GetSecond();
    }

    stmnts.emplace_back(std::move(result.GetFirst()));
  } while (!Peek(Token::RBrace));

  // the rhs location of the block is the right curly brace
  Location rhs_loc = loc;

  nexttok(); // eat '}'

  Location block_loc(left_loc.firstLine, left_loc.firstColumn, rhs_loc.lastLine,
                     rhs_loc.lastColumn);
  return {std::make_unique<Block>(block_loc, stmnts)};
}

/*
  term = conditional
       | while
       | bind
       | affix ";"
*/
auto Parser::ParseTerm(const Environment &env)
    -> Outcome<std::unique_ptr<Ast>, Error> {
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
  auto affix = ParseAffix(env);
  if (!affix) {
    return affix.GetSecond();
  }

  if (!Expect(Token::Semicolon)) {
    return {Error(Error::Code::MissingSemicolon, loc, "instead parsed " + txt)};
  }

  return {std::move(affix.GetFirst())};
}

/*
  conditional = "if" "(" affix ")" block "else" block
*/
auto Parser::ParseConditional(const Environment &env)
    -> Outcome<std::unique_ptr<Ast>, Error> {
  Location lhs_loc = loc;

  nexttok(); // eat "if"

  if (!Expect(Token::LParen)) {
    return Error(Error::Code::MissingLParen, loc);
  }

  auto test_term = ParseAffix(env);
  if (!test_term) {
    return {test_term.GetSecond()};
  }

  if (!Expect(Token::RParen)) {
    return Error(Error::Code::MissingRParen, loc);
  }

  if (!Peek(Token::LBrace)) {
    return Error(Error::Code::MissingLBrace, loc);
  }

  auto then_term = ParseBlock(env);
  if (!then_term) {
    return {then_term.GetSecond()};
  }

  if (!Expect(Token::Else)) {
    return Error(Error::Code::MissingElse, loc);
  }

  if (!Peek(Token::LBrace)) {
    return Error(Error::Code::MissingLBrace, loc);
  }

  auto else_term = ParseBlock(env);
  if (!else_term) {
    return {else_term.GetSecond()};
  }

  const Location &rhs_loc = else_term.GetFirst()->GetLoc();
  Location condloc(lhs_loc.firstLine, lhs_loc.firstColumn, rhs_loc.lastLine,
                   rhs_loc.lastColumn);
  return {std::make_unique<Conditional>(
      condloc, std::move(test_term.GetFirst()), std::move(then_term.GetFirst()),
      std::move(else_term.GetFirst()))};
}

/*
  while = "while" "(" affix ")" block
*/
auto Parser::ParseWhile(const Environment &env)
    -> Outcome<std::unique_ptr<Ast>, Error> {
  Location lhs_loc = loc;
  nexttok(); // eat 'while'

  auto test_result = ParseAffix(env);
  if (!test_result) {
    return {test_result.GetSecond()};
  }
  auto &test = test_result.GetFirst();

  if (!Expect(Token::Do)) {
    return Error(Error::Code::MissingDo, loc);
  }

  auto body_term = ParseBlock(env);
  if (!body_term) {
    return body_term.GetSecond();
  }
  auto &body = body_term.GetFirst();

  const Location &rhs_loc = body_term.GetFirst()->GetLoc();
  Location whileloc(lhs_loc.firstLine, lhs_loc.firstColumn, rhs_loc.lastLine,
                    rhs_loc.lastColumn);
  return {std::make_unique<While>(whileloc, std::move(test), std::move(body))};
}

/*
  affix = composite "=" affix
        | composite "(" [affix {"," affix}] ")"
        | composite
*/
auto Parser::ParseAffix(const Environment &env)
    -> Outcome<std::unique_ptr<Ast>, Error> {
  auto composite_term = ParseComposite(env);
  if (!composite_term) {
    return {composite_term.GetSecond()};
  }
  auto &composite = composite_term.GetFirst();

  // composite "=" affix
  if (Peek(Token::Equals)) {
    return ParseAssignment(env, std::move(composite));
  }

  // composite "(" [affix {"," affix}] ")"
  if (Peek(Token::LParen)) {
    return ParseApplication(env, std::move(composite));
  }

  // composite
  return {std::move(composite)};
}

/*
  composite '=' affix
*/
auto Parser::ParseAssignment(const Environment &env,
                             std::unique_ptr<Ast> composite)
    -> Outcome<std::unique_ptr<Ast>, Error> {
  nexttok(); // eat "="

  auto rhs_term = ParseAffix(env);
  if (!rhs_term) {
    return rhs_term.GetSecond();
  }
  auto &rhs = rhs_term.GetFirst();

  const Location &lhs_loc = composite->GetLoc();
  const Location &rhs_loc = rhs_term.GetFirst()->GetLoc();
  Location assign_loc(lhs_loc.firstLine, lhs_loc.firstColumn, rhs_loc.lastLine,
                      rhs_loc.lastColumn);
  return {std::make_unique<Assignment>(assign_loc, std::move(composite),
                                       std::move(rhs))};
}

/*
  composite "(" [affix {"," affix}] ")"
*/
auto Parser::ParseApplication(const Environment &env,
                              std::unique_ptr<Ast> composite)
    -> Outcome<std::unique_ptr<Ast>, Error> {
  Location rhs_loc;
  nexttok(); // eat '('

  std::vector<std::unique_ptr<Ast>> args;

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

    } while (tok == Token::Comma);

    // finish parsing the argument list
    if (!Peek(Token::RParen)) {
      return Error(Error::Code::MissingRParen, loc);
    }

    rhs_loc = loc;

    nexttok(); // eat ')'
  } else {     // tok == Token::RParen

    rhs_loc = loc;

    nexttok(); // eat ')'
    // the argument list is empty, but this is still an application term.
  }

  const Location &lhs_loc = composite->GetLoc();
  Location app_loc(lhs_loc.firstLine, lhs_loc.firstColumn, rhs_loc.lastLine,
                   rhs_loc.lastColumn);
  return {std::make_unique<Application>(app_loc, std::move(composite),
                                        std::move(args))};
}

/*
  composite = accessor operator infix-parser
            | accessor
*/
auto Parser::ParseComposite(const Environment &env)
    -> Outcome<std::unique_ptr<Ast>, Error> {
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
auto Parser::ParseAccessor(const Environment &env)
    -> Outcome<std::unique_ptr<Ast>, Error> {
  auto left_term = ParseBasic(env);
  if (!left_term) {
    return left_term.GetSecond();
  }
  auto &left = left_term.GetFirst();

  do {
    if (Peek(Token::Dot)) {
      left_term = ParseDot(env, std::move(left));
    } else if (Peek(Token::LBracket)) {
      left_term = ParseSubscript(env, std::move(left));
    }

  } while (Peek(Token::Dot) || Peek(Token::LBracket));

  return {std::move(left)};
}

/*
 dot = basic "." basic
*/
auto Parser::ParseDot(const Environment &env, std::unique_ptr<Ast> left)
    -> Outcome<std::unique_ptr<Ast>, Error> {
  auto lhs_loc = left->GetLoc();
  nexttok(); // eat '.'

  auto right_term = ParseBasic(env);
  if (!right_term) {
    return {right_term.GetSecond()};
  }
  auto &right = right_term.GetFirst();

  const Location &rhs_loc = right->GetLoc();
  Location dotloc(lhs_loc.firstLine, lhs_loc.firstColumn, rhs_loc.lastLine,
                  rhs_loc.lastColumn);
  return {std::make_unique<Dot>(dotloc, std::move(left), std::move(right))};
}

/*
 subscript = basic "[" basic "]"
*/
auto Parser::ParseSubscript(const Environment &env, std::unique_ptr<Ast> left)
    -> Outcome<std::unique_ptr<Ast>, Error> {
  auto lhs_loc = left->GetLoc();
  nexttok(); // eat '['

  auto right_term = ParseBasic(env);
  if (!right_term) {
    return {right_term.GetSecond()};
  }
  auto &right = right_term.GetFirst();

  if (!Expect(Token::RBracket)) { // eat ']'
    return {Error(Error::Code::MissingRBracket, loc, txt)};
  }

  auto rhs_loc = loc;
  Location location(lhs_loc.firstLine, lhs_loc.firstColumn, rhs_loc.lastLine,
                    rhs_loc.lastColumn);
  return {
      std::make_unique<Subscript>(location, std::move(left), std::move(right))};
}

/*
    This is an implementation of an operator precedence parser
    going from this pseudo-code:
    https://en.wikipedia.org/wiki/Operator-precedence_parser
*/
auto Parser::ParseInfix(std::unique_ptr<Ast> lhs, Precedence precedence,
                        const Environment &env)
    -> Outcome<std::unique_ptr<Ast>, Error> {
  Outcome<std::unique_ptr<Ast>, Error> result(std::move(lhs));

  InternedString peek_str = nullptr;
  llvm::Optional<std::pair<InternedString, BinopLiteral *>> peek_opt;
  BinopLiteral *peek_lit = nullptr;

  //
  while ((tok == Token::Op) &&
         ((peek_str = env.operators->Intern(txt)) != nullptr) &&
         (peek_opt = env.binops->Lookup(peek_str)) && (peek_opt.has_value()) &&
         ((peek_lit = peek_opt->second) != nullptr) &&
         (peek_lit->precedence >= precedence)) {
    InternedString op_str = peek_str;
    BinopLiteral *op_lit = peek_lit;
    Location op_loc = loc;

    nexttok(); // eat the 'operator'

    Outcome<std::unique_ptr<Ast>, Error> rhs = ParseAccessor(env);
    if (!rhs) {
      return rhs.GetSecond();
    }

    while ((tok == Token::Op) &&
           ((peek_str = env.operators->Intern(txt)) != nullptr) &&
           (peek_opt = env.binops->Lookup(peek_str)) &&
           (peek_opt.has_value()) &&
           ((peek_lit = peek_opt->second) != nullptr) &&
           ((peek_lit->precedence > op_lit->precedence) ||
            ((peek_lit->associativity == Associativity::Right) &&
             (peek_lit->precedence == op_lit->precedence)))) {
      auto temp =
          ParseInfix(std::move(rhs.GetFirst()), peek_lit->precedence, env);
      if (!temp) {
        return temp.GetSecond();
      }
      // get overload resolution to select move assignment
      // rhs = Outcome<std::unique_ptr<Ast>, Error>(std::move(temp.GetFirst()));
      rhs = std::move(temp.GetFirst());
    }

    const Location &rhs_loc = rhs.GetFirst()->GetLoc();
    Location binop_loc(op_loc.firstLine, op_loc.firstColumn, rhs_loc.lastLine,
                       rhs_loc.lastColumn);
    result = Outcome<std::unique_ptr<Ast>, Error>(
        std::make_unique<Binop>(binop_loc, op_str, std::move(result.GetFirst()),
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
auto Parser::ParseBasic(const Environment &env)
    -> Outcome<std::unique_ptr<Ast>, Error> {
  switch (tok) {
  case Token::Id: {
    Location lhs_loc = loc;
    InternedString symbol = env.symbols->Intern(txt);

    nexttok(); // eat the identifier

    if (Peek(Token::ColonEq)) {
      return ParseBind(symbol, lhs_loc, env);
    }

    return {std::make_unique<Variable>(lhs_loc, symbol)};
  }

  // #RULE: an operator appearing in the basic position is a unop
  case Token::Op: {
    Location lhs_loc = loc;
    InternedString opr = env.operators->Intern(txt);

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

    const Location &rhs_loc = rhs.GetFirst()->GetLoc();
    Location unop_loc(lhs_loc.firstLine, lhs_loc.firstColumn, rhs_loc.lastLine,
                      rhs_loc.lastColumn);
    return {std::make_unique<Unop>(unop_loc, opr, std::move(rhs.GetFirst()))};
  }

  // #RULE a open paren appearing in basic position
  // is either a parenthesized affix expression or
  // a tuple literal.
  case Token::LParen: // "("
  {
    nexttok(); // eat the '('

    auto result = ParseAffix(env);
    if (!result) {
      return result.GetSecond();
    }

    if (Peek(Token::Comma)) {
      return ParseTuple(env, std::move(result.GetFirst()));
    }
    // else we parsed a parenthised expression
    if (!Expect(Token::RParen)) {
      return {Error(Error::Code::MissingRParen, loc, txt)};
    }

    return {std::move(result.GetFirst())};
  }

  // #RULE a bracket appearing in basic position is an array literal
  case Token::LBracket: // '['
  {
    Location lhs_loc = loc;
    nexttok(); // eat '['

    std::vector<std::unique_ptr<Ast>> members;

    do {
      if (Peek(Token::Comma)) {
        nexttok(); // eat ','
      }

      auto member = ParseAffix(env);
      if (!member) {
        return member.GetSecond();
      }

      members.emplace_back(std::move(member.GetFirst()));
    } while (tok == Token::Comma);

    if (tok != Token::RBracket) {
      return Error(Error::Code::MissingRBracket, loc, txt);
    }

    Location rhs_loc = loc;

    nexttok(); // eat ']'

    Location array_loc(lhs_loc.firstLine, lhs_loc.firstColumn, rhs_loc.lastLine,
                       rhs_loc.lastColumn);
    return {std::make_unique<Array>(array_loc, std::move(members))};
  }

  // #RULE Token::Nil at basic position is the literal nil
  case Token::Nil: {
    Location lhs_loc = loc;
    nexttok(); // eat 'nil'
    return {std::make_unique<Nil>(lhs_loc)};
  }

  // #RULE Token::Int at basic position is a literal integer
  case Token::Integer: {
    Location lhs_loc = loc;
    int value = std::stoi(txt);
    nexttok(); // eat '[0-9]+'
    return {std::make_unique<Integer>(lhs_loc, value)};
  }

  // #RULE Token::True at basic position is the literal true
  case Token::True: {
    Location lhs_loc = loc;
    nexttok(); // Eat "true"
    return {std::make_unique<Boolean>(lhs_loc, true)};
  }

  // #RULE Token::False at basic position is the literal false
  case Token::False: {
    Location lhs_loc = loc;
    nexttok(); // Eat "false"
    return {std::make_unique<Boolean>(lhs_loc, false)};
  }

  default: {
    return {Error(Error::Code::UnknownBasicToken, loc, txt)};
  }
  } // !switch(tok)
}

auto Parser::ParseTuple(const Environment &env,
                        std::unique_ptr<Ast> first_element)
    -> Outcome<std::unique_ptr<Ast>, Error> {

  const Location &lhs_loc = first_element->GetLoc();
  Outcome<std::unique_ptr<Ast>, Error> result = Error();
  std::vector<std::unique_ptr<Ast>> elements;
  elements.emplace_back(std::move(first_element));

  while (Expect(Token::Comma)) {
    result = Outcome<std::unique_ptr<Ast>, Error>(ParseAffix(env));

    if (!result) {
      return result.GetSecond();
    }

    elements.emplace_back(std::move(result.GetFirst()));
  }

  if (!Peek(Token::RParen)) {
    return Error(Error::Code::MissingRParen, loc, txt);
  }

  Location rhs_loc = loc;

  nexttok(); // eat ')'

  Location tupleloc(lhs_loc.firstLine, lhs_loc.firstColumn, rhs_loc.lastLine,
                    rhs_loc.lastColumn);
  return {std::make_unique<Tuple>(tupleloc, std::move(elements))};
}

/*
  type = "Nil"
       | "Int"
       | "Bool"
       | "(" type {"," type} ")"
       | "[" type ";" int "]"
       | "Ptr" "<" type ">"
*/
auto Parser::ParseType(const Environment &env) -> Outcome<Type *, Error> {
  switch (tok) {
  // #RULE Token::NilType is the type Nil
  case Token::NilType: {
    nexttok(); // eat 'Nil'
    return {env.types->GetNilType()};
    break;
  }

  // #RULE Token::IntegerType is the type Int
  case Token::IntegerType: {
    nexttok(); // Eat "Int"
    return {env.types->GetIntType()};
    break;
  }

  // #RULE Token::BooleanType is the type Bool
  case Token::BooleanType: {
    nexttok(); // Eat "Bool"
    return {env.types->GetBoolType()};
    break;
  }

  // #RULE Token::LParen predicts the type Tuple
  case Token::LParen: {
    nexttok(); // eat '('
    auto left = ParseType(env);

    if (!left) {
      return left;
    }

    if (tok == Token::Comma) {
      std::vector<Type *> elem_tys = {left.GetFirst()};
      do {
        nexttok(); // eat ','

        auto elem_ty = ParseType(env);
        if (!elem_ty) {
          return elem_ty;
        }

        elem_tys.push_back(elem_ty.GetFirst());
      } while (tok == Token::Comma);

      left = env.types->GetTupleType(elem_tys);
    }

    if (!Expect(Token::RParen)) {
      return Error(Error::Code::MissingRParen, loc, txt);
    }

    return left;
  }

  // #RULE Token::LBracket predicts the type Array
  case Token::LBracket: {
    nexttok(); // eat '['

    auto array_type_result = ParseType(env);

    if (!array_type_result) {
      return array_type_result;
    }

    auto &array_type = array_type_result.GetFirst();

    if (!Expect(Token::Semicolon)) {
      return {Error(Error::Code::MissingArraySemicolon, loc, txt)};
    }

    if (!Peek(Token::Integer)) {
      return {Error(Error::Code::MissingArrayNum, loc, txt)};
    }

    size_t num = std::stoi(txt);

    nexttok(); // eat '[0-9]+'

    if (!Expect(Token::RBracket)) {
      return {Error(Error::Code::MissingRBracket, loc, txt)};
    }

    return {env.types->GetArrayType(num, array_type)};
  }

  // #RULE Token::Ptr predicts the type of a pointer
  case Token::Ptr: {
    nexttok(); // eat "Ptr"

    // if we don't see "<" that's a misformed Ptr Type literal.
    if (!Peek(Token::Op) || (strcmp("<", txt.c_str()) != 0)) {
      return {Error(Error::MissingLessThan, loc, txt)};
    }

    auto type = ParseType(env);

    if (!type) {
      return type;
    }

    return {env.types->GetPointerType(type.GetFirst())};
  }

  default: {
    return Error(Error::Code::UnknownTypeToken, loc, txt);
  }
  }
}

} // namespace pink
