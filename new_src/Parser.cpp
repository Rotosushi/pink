#include <string>
#include <memory>

#include "Variable.hpp"
#include "Application.hpp"
#include "Bind.hpp"
#include "Binop.hpp"
#include "Unop.hpp"
#include "Nil.hpp"
#include "Boolean.hpp"
#include "Integer.hpp"
#include "Lambda.hpp"

#include "Parser.hpp"

namespace pink {

Parser::Parser(const Environment& env)
  : env(env), lexer(env), curtok(Token::End), curloc(), curtxt()
{

}

Parser::Reset()
{
  lexer.Reset();
  curtok = Token::End;
  curloc = Location();
  curtxt = std::string();
}

void Parser::NextToken()
{
  curtok = lexer.yylex();
  curloc = lexer.yyloc();
  curtxt = lexer.yytxt();
}

/*
  defines the set of distinct start tokens in the
  language. all expressions begin with one of these
  tokens, there is no way to write down an expression
  without one of these tokens being in the first position.
  infix expressions begin with the unary operator
  or with an object of some type or with an identifier,
  which is itself a reference to a location of some type.
  parens simply group terms together, the type is the same
  as the grouped terms type,
  parens define a hetergenous composite literal or type (tuple or records)
  and brackets define a homogeneous composite literal or type (arrays)
*/
bool Parser::IsPrimary(Token token)
{
  switch(token)
  {
    case Token::Nil:      case Token::True:         case Token::False:
    case Token::Integer:  case Token::Type_Boolean: case Token::Type_Integer:
    case Token::Type_Nil: case Token::Identifier:   case Token::Backslash:
    case Token::Operator: case Token::Lparen:       case Token::Lbrace:
    case Token::Lbracket:
      return true;
    default:
      return false;
  }
}

/*
  defines the set of tokens that can end an expression,
  if the parser finds itself in a state where we have fully
  parsed something and we are deciding if we want to
  continue or not, Ending tokens express to the parser
  that the term we were just parsing has ended.
  this corresponds to the semicolon token, but also to
  the rparen token which ends a grouping term.
  and to the lambda body token '=>' which denotes
  the end of the argument list
*/
bool Parser::IsEnding(Token token)
{
  switch(token)
  {
    case Token::Rparen: case Token::End: case Token::Body:
    case Token::Rbrace: case Token::Rbracket:
      return true;
    default:
      return false;
  }
}


InternedString Parser::IsBinop(std::string& optxt)
{
  InternedString op = env.interned_operators.InternString(optxt);

  if (!op)
    return op;

  // lookup returns an optional type
  auto op_exists = env.binops.Lookup(op);

  if (op_exists)
    return op;
  else
    return nullptr;
}


InternedString Parser::IsUnop(std::string& optxt)
{
  InternedString op = env.interned_operators.InternString(optxt);

  if (!op)
    return op;

  // lookup returns an optional type
  auto op_exists = env.unops.Lookup(op);

  if (op_exists)
    return op;
  else
    return nullptr;
}

Judgement Parser::Parse(const std::string& data)
{
  lexer.SetBuffer(data);

  NextToken(); // prime our parser.

  Judgement result = ParseTerm(env);

  Reset();

  return result;
}

/*
  term := affix
        | affix ';' affix // except we dont have semicolons quite yet

  this also makes this the best place for this discussion,
  even though it's also relevant to every point we recurse
  up the parse tree while parsing.

  basically, the distinction between recursing to Term vs
  recursing to Affix is if the sub element of the Ast
  is a full blown other scope or if we are talking about
  a continuation of a term making up a larger expression.


  recursing to affix means that the next semicolon is going to
  act as a period for the entire recursive tree we have built
  up at this point. whereas recursing to term starts a whole
  new tree which can pick up compositions of expressions
  (semicolons).

  example:
   if we have a lambda that recurses to affix for it's body term

  the expression

  f0 := \x:Nil=>x ;

  the entire bind expression is considered ended once we hit the semicolon.

  whereas with a lambda that recurses to term,

  f0 := \x:Nil=>x ;

  the bind expression will never see any semicolon. as it will be eaten
  by the parser when it parses the body of the lambda.

  this has ramifications for building up lambda's within larger expressions.


  for now however, lambda's recurse up to Affix, and then we can imagine
  a sequence of definitions of lambda's separated by semicolons, which
  seems far more natural.

  with the option to use an explicit scope which allows a sequence
  of terms to become the lambda's body.

  this is the same-ish choice for (byref v. byval)
  the choice makes a valid language either way, it's
  a choice of which semantics are going to be present
  within the language, as we always have to design
  around the consequences of our choices. the solution
  to our first discussion ended up being
  we pass everything by-val, and provide syntax to
  pass by-ref explicitly. here we are talking about
  do we want the scope of a lambda term to be a fully
  fledged new -syntactic- scope (conceptually it is a full
  fledged scope either way), and as such do we dictate
  the flow of the parse along the lines of that new scope
  being introduced? and the thing is, if we parse it as a
  new scope, then adding the explicit scope symbols is redundant,
  and it makes defining a lambda within the body of another
  lambda cumbersome in all cases except currying.
  the programmer is forced to enclose the lambda
  definition in parenthesis to get access to a 'period' (i.e. ')') for
  the lambda's scope within it's defining scope.
  whereas if we treat the body like a continuation of the same affix
  expression, then the semicolon (':') act's as a period for the lambda
  even if the programmer doesn't begin the definintion with '('.
  and, this definition means the explicit scope syntax is adding
  legitimate utility to the grammar that would not be possible
  within the syntax of the language otherwise.
  (this is were we differ from the by-ref v. by-val discussion
   as whichever we picked in that discussion we would have to
   add syntax for the other, and neither choice stomped out any
   further descisions directly, within the currently discussion
   in one case we lose the ability to define a semantic,
   namely (A) the period for the lambda term itself, within the
   context of the defining scope, and in the other case (B)
   we do not directly define the missing case, that is
   if the semicolon act's as a period for the affix expression
   we are forced to define new syntax to declare a lambda with
   more than one body term. but within case (A) we
   preclude ourselves from declaring a lambda as a small syntactic
   element, exactly because we force it's body to always be able to pick up
   a composition of terms.


  (formal function definitions have their own token sequence
   which makes the explicit scope required, thus avoiding the
   issue.)
*/
Judgement Parser::ParseTerm()
{
  Judgement result = ParseAffix();
  return result;
}

/*
  affix := application
         | application '<-' affix // this is assignment.
         | application operator affix // except we use an operator precedence parser here
                                      // which is technically a subset of LR languages.
*/
Judgement Parser::ParseAffix()
{
  Judgement result;
  Location  lhsloc     = curloc; // buffer the location at this point in time.
  Judgement left_jdgmt = ParseApplication();

  // did we parse a valid beginning term?
  if (left_jdgmt)
  {
    // are we parsing an assignment or some other binary expression?

    if (curtok == Token::Assign)
    {
      NextToken(); // eat '<-'

      Judgement right_jdgmt = ParseAffix(); // we recurse here to describe nested assignment.

      // did we parse a valid right hand side?
      if (right_jdgmt)
      {
        // we have both sides of an assignment.
        Location rhsloc    = curloc;
        Location assignloc = Location(lhsloc.first_line,
                                      lhsloc.first_column,
                                      rhsloc.first_line,
                                      rhsloc.first_column);

        result = Judgement(std::make_shared<Assignment>(assignloc, left_jdgmt.term, right_jdgmt.term));
      }
      else
      {
        // the result is now the error from the right hand side.
        result = right_jdgmt;
      }
    }
    else if (curtok == Token::FnType) // TODO:: make this more general for composite types
    {
      NextToken(); // eat '->'


    }
    else if (curtok == Token::Operator)
    {
      result = ParseInfix(left_jdgmt.term, static_cast<Precedence>(0));
    }
    // it's okay to have a term
    // not be followed by anything.

  }
  else
  {
    // the result is now the error from the left hand side
    result = left_jdgmt;
  }
  // return the success or failure.
  return result;
}

/*
  application := primary
               | primary application
*/
Judgement Parser::ParseApplication()
{
  Judgement result;
  Location  lhsloc     = curloc;
  Judgement left_jdgmt = ParsePrimary();

  if (left_jdgmt)
  {
    std::vector<std::shared_ptr<Ast>> arguments;

    // this first check makes total sense,
    // we want to handle binops within the outer
    // scope to this one, ParseAffix, so we
    // obviously check that it isn't an operator,
    // and it is one of our Primary tokens.
    // but the second check is weird, if the lower
    // scope knows how to parse unops, shouldn't
    // this check be IsUnop?
    // great question, no. because if we allow the
    // lower scope to parse an operator we find
    // in this position as a unop, we will convert
    // what was meant to be a binary expression
    // into an application. (3 - 3) will parse
    // as (3 (-3)) which fails to run because we cannot
    // call numbers like we can call functions.
    // in essence, every binop has precedence over
    // any unop it shares a symbol with.
    while ((curtok != Token::Operator && IsPrimary(curtok))
         || curtok == Token::Operator && !IsBinop(curtok))
    {
      // why do we not recurse here?
      // because if you want the arguments
      // to an application to be the result
      // of another application, that's what
      // parenthesis are for.
      Judgement right_jdgmt = ParsePrimary();

      if (right_jdgmt)
      {
        arguments += right_jdgmt.term;
      }
      else
      {
        // the result is now the error from this argument.
        result = right_jdgmt;
        break;
      }
    }

    Location rhsloc = curloc;
    Location apploc = Location(lhsloc.first_line, lhsloc.first_column, rhsloc.first_line, rhsloc.first_column);
    // we ran into a token that we need to handle elsewhere,
    // but we have this application term to return.
    result = Judgement(std::make_shared<Application>(apploc, left_jdgmt.term, arguments));
  }
  else
  {
    // the result is now the error from the left_jdgmt
    result = left_jdgmt;
  }

  // unified return point.
  return result;
}

/*
  a binary operator precedence parser.

  pseudocode:

  OpPrecParser(lhs, min_precedence)
  {
    lookahead = peek next token

    while lookahead is a binary operator whose precedence is >= min_precedence
    {
      op = lookahead;

      advance to next token;

      rhs = ParsePrimary(env);

      lookahead = peek next token;

      while lookahead is a binary operator whose precedence is greater
            than op's, or is right associative with precedence equal to ops'
      {
        rhs = ParseInfix (rhs, precedence(lookahead));
        lookahead = peek next token;
      }

      lhs = binop lhs rhs;
    }

    return lhs;
  }
*/
Judgement Parser::ParseInfix(std::shared_ptr<Ast> left, Precedence min_precedence)
{
  Location lhsloc = curloc;
  Judgement result;

  if (InternedString op = IsBinop(curtxt))
  {
    // std::optional<std::pair<Associativity, Precedence>>
    auto maybeLookaheadAssocPrec = env.binop_precedence_table.Lookup(op);

    while (maybeLookaheadAssocPrec && (*maybeLookaheadAssocPrec).second >= min_precedence)
    {

      // save the previous operators precedence and associativity.
      std::pair<Associativity, Precedence> opAssocPrec = *maybeLookaheadAssocPrec;

      NextToken(); // eat the operator

      // we explicitly do not want to recur all the way back up
      // to affix here, unlike nearly everywhere else in the grammar,
      // because the major difference between Application and Affix is
      // Affix would call into ParseInfix before it returned
      // to this first call point. I'd rather not think about what
      // effect that has on the resulting trees other than they must be
      // weird. they might break order of operations, idk.
      // each branch of the binary expression would be parsed
      // by another instance of this procedure. rather than by way of
      // the inbuilt recursion, which handles the depth of the
      // binary expression already. so this extra, weird recursion is
      // to be avoided in the working version.
      // TODO: experiment with this and a trace on for fun/learning. maybe
      // it works because each instance of this procedure would validly
      // parse each subsegment of the affix expression already?
      Judgement right = ParseApplication();

      if (!right)
        return right;

      if (curtok == Token::Operator)
      {
        InternedString Lop;
        // if the lookahead operator is a binop
        // whose precedence is greater than
        if ((Lop = IsBinop(curtxt))
         && (maybeLookaheadAssocPrec = env.binop_precedence_table.Lookup(Lop))
         && (((*maybeLookaheadAssocPrec).second > opAssocPrec.second)
          || ((*maybeLookaheadAssocPrec).second == opAssocPrec.second
           && (*maybeLookaheadAssocPrec).first  == Associativity::Right)))
        {
          do {
            // recur, passing in the previous right hand side as this
            // new Infix expressions left hand side. this also means
            // that the precedence of the current token is the
            // new min_precedence we use in the new Infix expr.
            right = ParseInfix(right.term, (*maybeLookaheadAssocPrec).second);

            if (!right)
              return right;

            // so, here is the place we need to run the init code again for
            // parsing another binop, this also becomes another descision point
            // for parsing more or failing or continuing onwards with a
            // successful parse.
            if (curtok == Token::Operator)
            {
              // we saw another operator, so we have to fill in the
              // cells with the new information for the next iteration.
              if (op = IsBinop(curtxt))
              {
                // the optional type encodes the success/failure so we
                // don't have to explicitly handle it right here.
                // instead we can delegate that case to the already
                // written part of the while look above.
                maybeLookaheadAssocPrec = env.binop_precedence_table.Lookup(op);
              }
              else
              {
                // if this is an operator between two terms
                // it must be defined as a binop already.
                std::string errtxt = "Operator has no known definition ~> " + curtxt;
                left = Judgement(Error(errtxt, lhsloc));
              }
            }
            // else nothing, a missing binop here simply means an
            // an end to the InfixExpression.
          } while ((Lop = IsBinop(curtxt))
                && (maybeLookaheadAssocPrec = env.binop_precedence_table.Lookup(Lop))
                && (((*maybeLookaheadAssocPrec).second > opAssocPrec.second)
                 || ((*maybeLookaheadAssocPrec).second == opAssocPrec.second
                  && (*maybeLookaheadAssocPrec).first  == Associativity::Right)))
        }
        else
        {
          std::string errtxt = "Operator has no known definition ~> " + curtxt;
          left = Judgement(Error(errtxt, lhsloc));
          break;
        }
      }
      // a missing operator here, after we already parsed
      // both halves of the binary expression simply
      // means an end to the binary expression.

      // we have a left hand side (we were given this from
      // the above scope) and a right hand side, (we just parsed
      // it in this scope) so we build a binop from the operator
      // between the sides, and the sides themselves.
      Location rhsloc = curloc;
      Location boploc(lhsloc.first_line, lhsloc.first_column, rhsloc.first_line, rhsloc.first_column);

      // assign to left here so we can build up the tree
      // iteratively within this loop.
      left = Judgement(std::make_shared<Binop>(boploc, left.term, right.term));
    }
  }
  else
  {
    std::string errtxt = "Operator has no known definition ~> " + curtxt;
    left = Judgement(Error(errtxt, lhsloc));
  }

  // unified return point
  return left;
}


/*
  primary := identifier (bind-annotation)?
           | nil
           | Nil
           | integer
           | Int
           | true
           | false
           | Bool
           | '(' affix ')'

  bind-annotation := ':' type ('<-' affix)?
                   | ':' '<-' affix
*/
Judgement Parser::ParsePrimary()
{
  Location lhsloc = curloc;
  Judgement result;

  if (IsPrimary(curtok))
  {
    switch(curtok)
    {
      // handle identifiers
      case Token::Identifier:
      {
        // intern the string
        InternedString identifier = env.interned_names.InternString(curtxt);
        NextToken(); // eat the identifier.

        if (curtok == Token::Colon)
        {
          Judgement type_jdgmt, init_jdgmt;
          bool typed = false;
          // this is a type annotation for a declaration.
          NextToken() // eat ':'

          // optional type annotation.
          if (is_type(curtok))
          {
            typed      = true; // we have a type annotation.
            type_jdgmt = ParseAffix(); // parse a type annotation

            if (!type_jdgmt)
            {
              return type_jdgmt;
            }
          }

          // an initializer expression is optional
          // iff we have an explicit type.
          // otherwise this isn't a declaration it's a reference
          if (curtok == Token::Assign)
          {
            // we saw the predictor token for an initialization
            // expression, so we parse one.
            init_jdgmt = ParseAffix();

            if (!init_jdgmt)
              return init_jdgmt;
          }
          else if (typed)
          {
            // we saw a type annotation but not an intialization expression.
            // we are going to build a default object for initialization
            init_jdgmt = env.BuildZeroedValue(type_jdgmt.term);

            if (!init_jdgmt)
              return init_jdgmt;
          }
          else
          {
            // we saw the predictor for a declaration,
            // but we didn't see a type annotation, or
            // an initialization expression. this is an
            // error.
            Location errloc(lhsloc.first_line, lhsloc.first_column, curloc.first_line, curloc.first_column);
            std::string errtxt = "Missing a type annotation or an initialization expression for :" + std::string(identifier);
            result = Judgement(Error(errtxt, errloc));
          }
        }
        else
        {
          // this is a reference.
          Location rhsloc = curloc;
          Location varloc(lhsloc.first_line, lhsloc.first_column, rhsloc.first_line, rhsloc.first_column);
          // this is simply a reference/variable
          result = Judgement(std::make_shared<Variable>(varloc, identifier, env.symbols.GetScope()))
        }

        break;
      }

      case Token::Nil:
      {
        NextToken(); // eat 'nil'
        Location nilloc(lhsloc.first_line, lhsloc.first_column, curloc.first_line, curloc.first_column);
        result = Judgement(std::make_shared<Nil>(loc));
        break;
      }

      case Token::True:
      {
        NextToken(); // eat 'true'
        Location trueloc(lhsloc.first_line, lhsloc.first_column, curloc.first_line, curloc.first_column);
        result = Judgement(std::make_shared<Boolean>(trueloc, true));
        break;
      }

      case Token::False:
      {
        NextToken(); // eat 'false'
        Location falseloc(lhsloc.first_line, lhsloc.first_column, curloc.first_line, curloc.first_column);
        result = Judgement(std::make_shared<Boolean>(falseloc, false));
        break;
      }

      case Token::Integer:
      {
        int64_t value = std::stoi(curtxt); // convert the textual to an int64_t
        NextToken(); // eat the integer
        Location intloc(lhsloc.first_line, lhsloc.first_column, curloc.first_line, curloc.first_column);
        result = Judgement(std::make_shared<Integer>(intloc, value));
        break;
      }

      case Token::Type_Nil:
      {
        NextToken(); // eat 'Nil'
        Location nilloc(lhsloc.first_line, lhsloc.first_column, curloc.first_line, curloc.first_column);
        result = Judgement (std::make_shared<TypeLiteral>(loc, env.GetNilType()));
        break;
      }

      case Token::Type_Boolean:
      {
        NextToken(); // eat "Bool"
        Location boolloc(lhsloc.first_line, lhsloc.first_column, curloc.first_line, curloc.first_column);
        result = Judgement (std::make_shared<TypeLiteral>(loc, env.GetBooleanType()));
        break;
      }

      case Token::Type_Integer:
      {
        NextToken(); // eat 'Int'
        Location intloc(lhsloc.first_line, lhsloc.first_column, curloc.first_line, curloc.first_column);
        result = Judgement (std::make_shared<TypeLiteral>(loc, env.GetIntegerType()));
        break;
      }

      // '\' formal-args '=>' affix
      case Token::Backslash:
      {
        result = ParseLambda();
        break;
      }

      // operator affix
      case Token::Operator:
      {
        if (InternedString op = IsUnop(curtxt))
        {
          NextToken(); // eat the operator

          if (IsPrimary(curtok))
          {
            Judgement right_jdgmt = ParsePrimary();

            if (right_jdgmt)
            {
              result = Judgement(std::make_shared<Unop>());
            }
            else
            {
              result = right_jdgmt;
            }
          }
          else
          {
            Location errloc(lhsloc.first_line, lhsloc.first_column, curloc.first_line, curloc.first_column);
            std::string errtxt = "Unop not followed by a primary token ~> " + TokenToString(curtok);
            result = Judgement(Error(errtxt, errloc));
          }
        }
        break;
      }

      case Token::Lparen:
      {
        NextToken(); // eat '('
        Judgement paren_jdgmt;

        if (curtok == Token::Rparen) // '()' is Nil
        {
          Location nilloc(lhsloc.first_line, lhsloc.first_column, curloc.first_line, curloc.first_column);
          paren_jdgmt = Judgement(std::make_shared<Nil>(nilloc));
        }
        else
        {
          paren_jdgmt = ParseAffix();
        }

        if (paren_jdgmt)
        {
          if (curtok == Token::Rparen)
          {
            NextToken(); // eat ')'
            result = paren_jdgmt;
          }
          // else if (curtok == Token::Comma) {...} // build a tuple
          else
          {
            Location errloc(lhsloc.first_line, lhsloc.first_column, curloc.first_line, curloc.first_column);
            std::string errtxt = "No matching closing Parenthesis";
            result = Judgement(Error(errtxt, errloc));
          }
        }
        else
        {
          result = paren_jdgmt;
        }
        break;
      }

      // since we should be keeing IsPrimary and this procedure
      // in lockstep with eachother, this becomes a FatalError.
      default:
        FatalError("Unsupported Primary Token ~>" + TokenToString(curtok), __FILE__, __LINE__);
    }
  }


  return result;
}

Judgement Parser::ParseLambda()
{
  Location lhsloc = curloc;
  Judgement result;
  std::vector<std::pair<InternedString, llvm::Type*>> formal_arguments;

  // builds and inserts a single argument
  // into the formal argument list.
  // on success returns a true judgement holding nothing.
  // on failure returns a false judgement and the error.
  auto ParseFormalArgument = [&]() -> Judgement
  {
    /* id ':' type */
    if (curtok == Token::Identifier)
    {
      InternedString op = env.interned_names.InternString(curtxt);
      NextToken(); // eat identifier

      if (curtok == Token::Colon)
      {
        NextToken(); // eat ':'

        Judgement type_jdgmt = ParseAffix();

        if (type_jdgmt)
        {
          TypeLiteral* type_literal = llvm::dyn_cast<TypeLiteral>(type_jdgmt.get());

          if (type_literal)
          {
            llvm::Type* type = type_literal->GetLiteral();

            formal_arguments.emplace_back(op, type);

            return Judgement (std::shared_ptr<Ast>());
          }
          else
          {
            std::string errtxt = "Type annotation must be written with Types."
            Location    errloc(lhsloc.first_line, lhsloc.first_column, curloc.first_line, curloc.first_column);
            return Judgement(Error(errtxt, errloc));
          }
        }
        else
        {
          return type_jdgmt;
        }
      }
      else
      {
        std::string errtxt = "Formal Argument must have a Type annotation."
        Location    errloc(lhsloc.first_line, lhsloc.first_column, curloc.first_line, curloc.first_column);
        return Judgement(Error(errtxt, errloc));
      }
    }
    else
    {
      std::string errtxt = "Formal Argument must have a name.";
      Location    errloc(lhsloc.first_line, lhsloc.first_column, curloc.first_line, curloc.first_column);
      return Judgement(Error(errtxt, errloc));
    }
  }

  if (curtok != Token::Backslash)
    FatalError("We switched on a Token::Backslash to get here, how did this happen?", __FILE__, __LINE__);

  NextToken(); // eat '\'

  if (curtok == Token::Identifier)
  {
    Judgement arg_jdgmt = ParseFormalArgument();

    if (arg_jdgmt)
    {
      if (curtok == Token::Comma)
      {
        // loop and parse each formal argument.
        do {
          NextToken(); // eat ','
          arg_jdgmt = ParseFormalArgument();

          if (!arg_jdgmt)
          {
            result = arg_jdgmt;
            break;
          }
        } while (curtok == Token::Comma);
      }

      if (curtok == Token::Body)
      {
        NextToken(); // eat '=>'

        Judgement body_jdgmt = ParseAffix();

        if (body_jdgmt)
        {
          Location lamloc(lhsloc.first_line, lhsloc.first_column, curloc.first_line, curloc.first_column);
          result = Judgement(std::make_shared<Lambda>(lamloc, formal_arguments, env.symbols, body_jdgmt.term));
        }
        else
        {
          result = body_jdgmt;
        }
      }
      else
      {
        std::string errtxt = "Missing a '=>' after the formal argument list";
        Location errloc(lhsloc.first_line, lhsloc.first_column, curloc.first_line, curloc.first_column);
        result = Judgement(Error(errtxt, errloc));
      }

    }
    else
    {
      result = arg_jdgmt;
    }
  }
  else
  {
    // if there isn't a programmer specified argument, we assume
    // nil is the argument type, and we generate a name.
    // this allows the programmer to type \=>{...}
    // and call the lambda with nil as the first argument,
    // that is '()'
    if (curtok == Token::Body)
    {
      NextToken();
      InternedString arg_name = env.interned_names.InternString(Gensym());
      llvm::Type*    arg_type = env.GetNilType();

      formal_arguments.emplace_back(arg_name, arg_type);

      Judgement body_jdgmt = ParseAffix();

      if (body_jdgmt)
      {
        Location lamloc(lhsloc.first_line, lhsloc.first_column, curloc.first_line, curloc.first_column);
        result = Judgement(std::make_shared<Lambda>(lamloc, formal_arguments, env.symbols, body_jdgmt.term));
      }
      else
      {
        result = body_jdgmt;
      }
    }
    else
    {
      std::string errtxt = "Missing a '=>' after the lambda declaration";
      Location errloc(lhsloc.first_line, lhsloc.first_column, curloc.first_line, curloc.first_column);
      result = Judgement(Error(errtxt, errloc));
    }
  }

}


}






































// --
