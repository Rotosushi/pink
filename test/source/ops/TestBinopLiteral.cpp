#include "ops/TestBinopLiteral.h"
#include "Test.h"
#include "ops/BinopLiteral.h"

#include "aux/Environment.h"

#include "support/DisableWarning.h"

// NOTE: 9/15/2022
// This function is a stub which is only defined for the sake of the
// test within this file, and is not expected to have -any- functionality.
// as such we do not use all of the parameters. since this is expected,
// we are choosing to disable this specific warning just for the function.
NOWARN(
    "-Wunused-parameter",
    pink::Outcome<llvm::Value *, pink::Error> test_binop_literal_fn(
        llvm::Type *lty, llvm::Value *left, llvm::Type *rty, llvm::Value *right,
        const pink::Environment &env) { return {pink::Error()}; })

bool TestBinopLiteral(std::ostream &out) {
  bool result = true;
  out << "\n-----------------------\n";
  out << "Testing pink::BinopLiteral: \n";

  auto options = std::make_shared<pink::CLIOptions>();
  auto env = pink::Environment::NewGlobalEnv(options);

  pink::Type *ty = env->types->GetIntType();
  pink::Precedence p = 5;
  pink::Associativity a = pink::Associativity::Left;
  pink::BinopLiteral binop(p, a);

  result &= Test(out, "BinopLiteral::precedence", binop.precedence == p);
  result &= Test(out, "BinopLiteral::associativity", binop.associativity == a);

  auto pair = binop.Register(ty, ty, ty, test_binop_literal_fn);

  result &= Test(out, "BinopLiteral::Register()",
                 pair.first == std::make_pair(ty, ty));

  auto opt = binop.Lookup(ty, ty);

  result &= Test(out, "BinopLiteral::Lookup()",
                 opt.has_value() && (*opt).first == std::make_pair(ty, ty));

  binop.Unregister(ty, ty);

  opt = binop.Lookup(ty, ty);

  result &= Test(out, "BinopLiteral::Unregister()", !opt.has_value());

  result &= Test(out, "pink::BinopLiteral", result);
  out << "\n-----------------------\n";
  return result;
}
