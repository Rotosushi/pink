#include "support/Common.h"

/*
   getAstOrNull exists convert Outcome<...> into an Ast*
   with a single line of code. which reduces boilerplate
   in the tests
*/
pink::Ast *
GetAstOrNull(pink::Outcome<std::unique_ptr<pink::Ast>, pink::Error> &outcome) {
  if (outcome)
    return outcome.GetFirst().get();
  else
    return nullptr;
}
