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
#include "ast/Bool.h"
#include "ast/Int.h"
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
  if (input_stream == nullptr) {
    FatalError("istream cannot be nullptr when initializing the Parser. text "
               "cannot be extracted from nowhere.",
               __FILE__, __LINE__);
  }
}

auto Parser::GetIStream() const -> std::istream * { return input_stream; }

void Parser::SetIStream(std::istream *stream) {
  if (stream == nullptr) {
    FatalError("istream cannot be nullptr when setting the input stream of the "
               "Parser.",
               __FILE__, __LINE__);
  }

  input_stream = stream;
}

auto Parser::EndOfInput() const -> bool {
  return lexer.EndOfInput() && input_stream->eof();
}

auto Parser::Getline() const -> std::string {
  std::string buf;
  char cursor = '\0';

  do {
    cursor = static_cast<char>(input_stream->get());

    if (input_stream->eof()) // don't append the EOF character
    {
      break;
    }

    buf += cursor; // append the character even when it's '\n'

  } while (cursor != '\n');

  return buf;
}

void Parser::yyfill() {
  std::string buf = Getline();
  lexer.AppendBuf(buf);
}

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

  size_t lines_seen = 1; // we start counting lines at 1

  // while we haven't reached the line we are looking for
  while (lines_seen < loc.firstLine) {

    if (*cursor == '\n') {
      lines_seen++;
    }

    cursor++; // keep looking
  }

  // if we didn't reach the end of the buffer
  if (cursor != end) {
    // find the end of the line cursor points to
    auto eol = cursor;

    while (eol != end && *eol != '\n') {
      eol++;
    }

    // eol points to end or to a newline char
    // either way the constructor we are using
    // uses to range [first, last), so we will
    // not copy the newline into the new string.

    // return the string between cursor and eol
    return {cursor, eol};
  }
  // else we could not find the requested line
  // so we return the empty string.
  return {};
}

/*
 *
 *  okay, i need to think about this, and I don't really know
 *  where to put this comment, but where exactly do we place
 *  calls to CreateLoad instructions?
 *
 *  we used to simply load the value of a variable when it was
 *  referenced within the text. which was fine, because we didn't
 *  have pointers before.
 *  the reason adding pointers is problematic is because each
 *  AllocaInst/GlobalVariable is already a pointer.
 *  and since each value we needed to deal with was the value
 *  the llvm::pointer pointed too, this was fine.
 *
 *  but the question becomes what about when the value we want
 *  to retrieve is the llvm::pointer's value itself?
 *  as in the case of a value representing a pointer to some
 *  memory.
 *
 *  i suppose what we want is two implementations of
 *  Variable::Codegen. one where we load before returning
 *  the result of the load, which should happen if the type
 *  of this variable is a pointer, or we should simply
 *  return the llvm::Value* itself, in the case where
 *  the variable is a pointer itself.
 *
 *  then, addition on pointers could be acheived
 *  by the same instruction, except we pass in
 *  the pointers themselves as values instead of a
 *  loadInst. I guess that means this comment belongs in
 *  pink::Variable.
 *
 *  okay, this solution works great for the times when we are
 *  referencing the variable as it's value.
 *  however, when we want to assign a new value into the
 *  variable's location, we don't want to have already loaded it,
 *  instead we simply want the ptr referencing the memory again,
 *  like in the previous case.
 *
 *  however, how can we communicate that to Variable::Codegen?
 *
 *  well, the first thing i notice is that this is suspiciously
 *  like c's lvalues and rvalues in their semantics.
 *
 *  the value on the left is being assigned, so we want the address,
 *  the value on the right is being used and so we want to load
 *  it's value into a register.
 *
 *  we might be able to store a bit in the environment? which is
 *  kinda weird, however, we can set the bit only when we codegen
 *  an assignment's lhs.
 *  and due to the way the parser works, nested assignment terms
 *  will construct tree's like:
 *
 *  a = b = c = ...;
 *
 *         =
 *       /   \
 *      a      =
 *           /   \
 *          b      =
 *               /   \
 *              c     ...;
 *
 * which means if we unset the bit after codegening the lhs we
 * will not treat the rhs as a value being assigned, and within
 * the rhs, we can have another assignment term with it's own lhs
 *
 * so, one thing that comes to mind is that we don't necessarily have
 * only variable terms on the lhs of an assignment,
 *
 * we can imagine terms which look like
 *
 * x + y = ...;
 *
 * where x is a pointer into an array,
 * and y is an offset.
 *
 * although, that would mean that x would have pointer type,
 * and thus we could make pointers exempt from this rule.
 * which would mean we didn't load x, however in this situation
 * we want to load y. and since it appears on the left, this
 * naive approach would cause us to not load it.
 *
 * since we don't allow assignment to literals,
 * the only case where we would validly assign to a temporary
 * value is if that value has pointer type.
 *
 * however, we wouldn't want to assign in the case of a term like:
 *
 * &var = ...;
 *
 * so, lets recap:
 *
 *  // "//+" means this syntax only makes sense if the temp value produced
 *  //       makes sense to assign to, and otherwise is a semantic error
 *
 *  1) x = ...; // we don't have any concept of const yet
 *  2) x op y = ...; //+
 *  3) op x = ...;   //+
 *  4) x(...) = ...; //+
 *
 *
 *
 *  okay, so what about following C a little bit again, with keeping track
 *  of the assignable state of each value within the language?
 *
 *  pointers to memory are assignable, that is
 *  llvm::AllocaInst/llvm::GlobalVariable
 *
 *  and regular values, that is any llvm::Value* that does not represent an
 *  allocation of memory which can be assigned into is not assignable.
 *
 *  then, we can modify operators to also do work on this assignable state,
 *  so, when an operation is performed, we can return either an assignable
 *  or an unassignable.
 *
 *  well, all that is fine, but it still doesn't change the fact that we
 *  have to load a value from a variable reference on the right side of
 *  assignment, and not load a value from a variable reference on the left
 *  side of assignment. if we had the variable marked as assignable, we
 *  would still need different behavior on that assignable variable
 *  depending on if it appeared on the left or the right hand side.
 *
 *  so i suppose we need a condition in Variable::Codegen which
 *  has the behavior we want depending on if we were on the left
 *  or the right hand side. but this solution needs to play nice
 *  with each of the possibilities above. for instance, if we
 *  were computing an offset into an array, by adding a pointer
 *  variable together with a integer variable, we would need to not
 *  load the pointer variable. (which we can handle by way of the fact
 *  that we treat pointer variables differently to all other variables)
 *  yet we would need to load the regular variable. even though it is on
 *  the left. We could even imagine a complex arithmetic expression
 *  including multiple values which computes the offset, and any variable
 *  appearing within that expression would need to be handled as we would
 *  with any arithmetic expression appearing on the right hand side.
 *
 *  similarly any arithmetic expression appearing within the argument
 *  position of a function call would need to be treated normally.
 *
 *  if we had a pointer to a pointer to some value, we could have an expression
 * like
 *
 *  *ptr = ...;
 *
 *  this would require the rhs to be of pointer type, obviously. but it
 *  would also require that we perform a load from the first pointer to
 *  get the second and then we would be modifying the second ptr.
 *  inside llvm (ptr) would be a pointer to the memory where the
 *  first pointer is stored, which would hold a value which is a pointer
 *  to the second location which is itself another pointer to a third
 *  location. the * operator should retreive the pointer type value
 *
 *  so i think this works naturally, iff the pointers themselves are
 *  unassignable, and then the * operator changes their type such that
 *  they become assignable.
 */

auto Parser::Parse(const Environment &env)
    -> Outcome<std::unique_ptr<Ast>, Error> {
  // prime the lexer with the first token from the input stream
  if (Peek(Token::End) && lexer.EndOfInput() && !input_stream->eof()) {
    nexttok();
  }

  // if tok == Token::End even after we tried to get more input from the given
  // file it means we read to the EOF, so we return the end of file error, to
  // signal to the caller that the end of the associated input has been reached.
  if (Peek(Token::End)) {
    return {Error(Error::Code::EndOfFile, loc)};
  }

  // Parse a single expression from the source file.
  return ParseTop(env);
}

/*
  top = function
      | variable
*/
auto Parser::ParseTop(const Environment &env)
    -> Outcome<std::unique_ptr<Ast>, Error> {
  if (Peek(Token::Fn)) {
    return ParseFunction(env);
  }

  if (Peek(Token::Var)) {
    return ParseBind(env);
  }
  /// \note  is is worth it to attempt to parse the bad expression
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
  variable = "var" id ":=" affix ";"
*/
auto Parser::ParseBind(const Environment &env)
    -> Outcome<std::unique_ptr<Ast>, Error> {
  Location lhs_loc = loc;
  InternedString symbol = nullptr;

  nexttok(); // eat "var"

  if (!Peek(Token::Id)) {
    return Error(Error::Code::MissingBindId, loc, "parsed: " + txt);
  }

  symbol = env.symbols->Intern(txt);

  nexttok(); // eat id

  if (!Expect(Token::ColonEq)) {
    return Error(Error::Code::MissingBindColonEq, loc, "parsed: " + txt);
  }

  auto affix = ParseAffix(env);
  if (!affix) {
    return {affix.GetSecond()};
  }

  if (!Peek(Token::Semicolon)) {
    return Error(Error::Code::MissingSemicolon, loc, "parsed: " + txt);
  }
  // the semicolon is what we consider to be the end of the bind term
  Location rhs_loc = loc;

  nexttok(); // eat ';'

  Location bind_loc(lhs_loc.firstLine, lhs_loc.firstColumn, rhs_loc.lastLine,
                    rhs_loc.lastColumn);
  return {
      std::make_unique<Bind>(bind_loc, symbol, std::move(affix.GetFirst()))};
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

  // todo: if (!isTypeToken(tok))
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
       | variable
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
  accessor = basic {"." basic}
           | basic {"[" basic "]"}
           | basic
*/
auto Parser::ParseAccessor(const Environment &env)
    -> Outcome<std::unique_ptr<Ast>, Error> {
  auto left_term = ParseBasic(env);
  if (!left_term) {
    return left_term.GetSecond();
  }
  auto &left = left_term.GetFirst();

  if (Peek(Token::Dot)) {
    return ParseDot(env, std::move(left));
  }

  if (Peek(Token::LBracket)) {
    return ParseSubscript(env, std::move(left));
  }

  return {std::move(left)};
}

/*
 dot = basic {"." basic}
*/
auto Parser::ParseDot(const Environment &env, std::unique_ptr<Ast> left)
    -> Outcome<std::unique_ptr<Ast>, Error> {
  auto lhs_loc = left->GetLoc();
  Outcome<std::unique_ptr<Ast>, Error> left_term = std::move(left);

  do {
    nexttok(); // eat '.'

    auto right = ParseBasic(env);
    if (!right) {
      return {right.GetSecond()};
    }

    const Location &rhs_loc = right.GetFirst()->GetLoc();
    Location dotloc(lhs_loc.firstLine, lhs_loc.firstColumn, rhs_loc.lastLine,
                    rhs_loc.lastColumn);
    left_term = Outcome<std::unique_ptr<Ast>, Error>(std::make_unique<Dot>(
        dotloc, std::move(left_term.GetFirst()), std::move(right.GetFirst())));
  } while (Peek(Token::Dot));

  return {std::move(left_term.GetFirst())};
}

/*
 subscript = basic {"[" basic "]"}
*/
auto Parser::ParseSubscript(const Environment &env, std::unique_ptr<Ast> left)
    -> Outcome<std::unique_ptr<Ast>, Error> {
  auto lhs_loc = left->GetLoc();
  Outcome<std::unique_ptr<Ast>, Error> left_term = std::move(left);

  do {
    nexttok(); // eat '['

    auto right_term = ParseBasic(env);
    if (!right_term) {
      return {right_term.GetSecond()};
    }

    if (!Expect(Token::RBracket)) { // eat ']'
      return {Error(Error::Code::MissingRBracket, loc)};
    }

    auto rhs_loc = loc;
    Location subscript_loc(lhs_loc.firstLine, lhs_loc.firstColumn,
                           rhs_loc.lastLine, rhs_loc.lastColumn);
    left_term =
        Outcome<std::unique_ptr<Ast>, Error>(std::make_unique<Subscript>(
            subscript_loc, std::move(left_term.GetFirst()),
            std::move(right_term.GetFirst())));
  } while (Peek(Token::LBracket));

  return {std::move(left_term.GetFirst())};
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
  while (TokenToBool(tok) && (tok == Token::Op) &&
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

    while (TokenToBool(tok) && (tok == Token::Op) &&
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
      rhs = Outcome<std::unique_ptr<Ast>, Error>(std::move(temp.GetFirst()));
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
  basic = id
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
      return {Error(Error::Code::MissingRParen, loc)};
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
      return Error(Error::Code::MissingRBracket, loc);
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
  case Token::Int: {
    Location lhs_loc = loc;
    int value = std::stoi(txt);
    nexttok(); // eat '[0-9]+'
    return {std::make_unique<Int>(lhs_loc, value)};
  }

  // #RULE Token::True at basic position is the literal true
  case Token::True: {
    Location lhs_loc = loc;
    nexttok(); // Eat "true"
    return {std::make_unique<Bool>(lhs_loc, true)};
  }

  // #RULE Token::False at basic position is the literal false
  case Token::False: {
    Location lhs_loc = loc;
    nexttok(); // Eat "false"
    return {std::make_unique<Bool>(lhs_loc, false)};
  }

  default: {
    return {Error(Error::Code::UnknownBasicToken, loc, "parsed: " + txt)};
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
    return Error(Error::Code::MissingRParen, loc);
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
       | "[" type "x" int "]"
       | "ptr" type
*/
auto Parser::ParseType(const Environment &env) -> Outcome<Type *, Error> {
  switch (tok) {
  // #RULE Token::NilType is the type Nil
  case Token::NilType: {
    nexttok(); // eat 'Nil'
    return {env.types->GetNilType()};
    break;
  }

  // #RULE Token::IntType is the type Int
  case Token::IntType: {
    nexttok(); // Eat "Int"
    return {env.types->GetIntType()};
    break;
  }

  // #RULE Token::BoolType is the type Bool
  case Token::BoolType: {
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
      return Error(Error::Code::MissingRParen, loc);
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

    if (!Peek(Token::Id) || txt != "x") {
      return {Error(Error::Code::MissingArrayX, loc, "parsed: " + txt)};
    }

    nexttok(); // eat 'x'

    if (!Peek(Token::Int)) {
      return {Error(Error::Code::MissingArrayNum, loc, "parsed: " + txt)};
    }

    size_t num = std::stoi(txt);

    nexttok(); // eat '[0-9]+'

    if (!Expect(Token::RBracket)) {
      return {Error(Error::Code::MissingRBracket, loc, "parsed: " + txt)};
    }

    return {env.types->GetArrayType(num, array_type)};
  }

  // #RULE Token::Ptr predicts the type of a pointer
  case Token::Ptr: {
    nexttok(); // eat "ptr"

    auto type = ParseType(env);

    if (!type) {
      return type;
    }

    return {env.types->GetPointerType(type.GetFirst())};
  }

  default: {
    return Error(Error::Code::UnknownTypeToken, loc, "parsed: " + txt);
  }
  }
}

} // namespace pink
