#include "ops/BinopTable.h"

namespace pink {

auto BinopTable::Register(InternedString opr, Precedence precedence,
                          Associativity associativity)
    -> std::pair<InternedString, BinopLiteral *> {
  auto iter = table.find(opr);

  if (iter == table.end()) {
    auto pair = table.insert(std::make_pair(
        opr, std::make_unique<BinopLiteral>(precedence, associativity)));
    return std::make_pair(pair.first->first, pair.first->second.get());
  }

  return std::make_pair(iter->first, iter->second.get());
}

auto BinopTable::Register(InternedString opr, Precedence precedence,
                          Associativity associativity, Type *left_t,
                          Type *right_t, Type *ret_t, BinopCodegenFn fn_p)
    -> std::pair<InternedString, BinopLiteral *> {
  auto iter = table.find(opr);

  if (iter == table.end()) {
    auto pair = table.insert(std::make_pair(
        opr, std::make_unique<BinopLiteral>(precedence, associativity, left_t,
                                            right_t, ret_t, fn_p)));
    return std::make_pair(pair.first->first, pair.first->second.get());
  }
  // register the new overload to the already registered binop
  // as a convienience for the caller of this procedure.
  iter->second->Register(left_t, right_t, ret_t, fn_p);
  return std::make_pair(iter->first, iter->second.get());
}

void BinopTable::Unregister(InternedString opr) {
  auto iter = table.find(opr);

  if (iter != table.end()) {
    table.erase(iter);
  }
}

auto BinopTable::Lookup(InternedString opr)
    -> llvm::Optional<std::pair<InternedString, BinopLiteral *>> {
  auto iter = table.find(opr);

  if (iter == table.end()) {
    // we want to be able to parse binops which have yet to be defined,
    // so we don't put the parser in a state of having partially parsed
    // a term in the situation where we are parsing a binop expression
    // that has a binop which has yet to be defined. so, we construct a
    // default implementation of the binop here. Note that this doesn't
    // define any implementation of the binop, so this is truly just to
    // move the error of an undefined binop out of the middle of the
    // parsers subroutines.
    // if you are thinking about this, you will notice a problem, which
    // is that the programmer may define the binop at a later point with
    // different precedence and associativity, which means we have to
    // reparse the binop expression after the point at which we encounter
    // the definition.
    // while this is true, it is a moot point until we add user defined
    // operators. after that, yes. and in fact, part of the solution is
    // going to be the parser storing something which keeps track of the
    // text associated with any given term which is constructed by the
    // parse. so we can then reparse that text to build a properly formed
    // term according to the actual precedence and associativity.
    // (we could imagine that something being a pair of
    // std::string::iterator's per definition, where the string they
    // point into is the lexer's buffer of the input text that it has
    // lexed, then we can construct a string at a later point which
    // represents that precise term.)
    // the other part of this problem is that now we have to distinguish
    // between user defined binops, and a default defined binop, so we
    // can allow the overwriting of default binops, and disallow the
    // overwriting of user defined binops. this however is simple to
    // solve with a boolean in each binop definition.
    // it is also fortunate that use-before-definition of variables does
    // not share this issue, because the shape of the tree which
    // represents said variable does not change based upon it's
    // definition, unlike binops expressions.
    //
    // the default precedence is 3 (simply to be the same as addition, because
    // that is a familiar precedence for everyone)
    // the default associativity is left, because that is nearly the
    // default for real world operators.
    return {Register(opr, 3, Associativity::Left)};
  }
  return {std::make_pair(iter->first, iter->second.get())};
}
} // namespace pink
