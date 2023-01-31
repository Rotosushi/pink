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
    llvm::Value *test_binop_literal_fn(llvm::Type *lty, llvm::Value *left,
                                       llvm::Type *rty, llvm::Value *right,
                                       const pink::Environment &env) {
      return nullptr;
    })

bool TestBinopLiteral(std::ostream &out) {
  bool        result = true;
  std::string name   = "pink::BinopLiteral";
  TestHeader(out, name);

  auto options = std::make_shared<pink::CLIOptions>();
  auto env     = pink::Environment::NewGlobalEnv(options);

  pink::Type::Pointer ty = env->types->GetIntType();
  pink::Precedence    p  = 5;
  pink::Associativity a  = pink::Associativity::Left;
  pink::BinopLiteral  binop(p, a);

  result &=
      Test(out, "BinopLiteral::GetPrecedence()", binop.GetPrecedence() == p);
  result &= Test(out, "BinopLiteral::GetAssociativity()",
                 binop.GetAssociativity() == a);

  auto pair = binop.Register(ty, ty, ty, test_binop_literal_fn);

  result &= Test(out, "BinopLiteral::Register()",
                 pair.first == std::make_pair(ty, ty));

  auto opt = binop.Lookup(ty, ty);

  result &= Test(out, "BinopLiteral::Lookup()",
                 opt.has_value() && (*opt).first == std::make_pair(ty, ty));

  binop.Unregister(ty, ty);

  opt = binop.Lookup(ty, ty);

  result &= Test(out, "BinopLiteral::Unregister()", !opt.has_value());

  return TestFooter(out, name, result);
}
