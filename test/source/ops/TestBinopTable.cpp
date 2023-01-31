#include "Test.h"

#include "ast/Binop.h"

#include "ops/BinopTable.h"
#include "ops/TestBinopTable.h"

#include "aux/Environment.h"

#include "support/DisableWarning.h"

// NOTE: 9/15/2022
// This function is a stub which is only defined for the sake of the
// test within this file, and is not expected to have -any- functionality.
// as such we do not use all of the parameters. since this is expected,
// we are choosing to disable this specific warning just for the function.
NOWARN(
    "-Wunused-parameter",
    llvm::Value *test_binop_table_fn(llvm::Type *lty, llvm::Value *left,
                                     llvm::Type *rty, llvm::Value *right,
                                     const pink::Environment &env) {
      return nullptr;
    })

bool TestBinopTable(std::ostream &out) {
  bool        result = true;
  std::string name   = "pink::BinopTable";
  TestHeader(out, name);

  auto options = std::make_shared<pink::CLIOptions>();
  auto env     = pink::Environment::NewGlobalEnv(options);

  pink::InternedString plus = env->operators->Intern("+");
  pink::Type::Pointer  ty   = env->types->GetIntType();
  pink::BinopTable     binop_table;
  pink::Precedence     p = 5;
  pink::Associativity  a = pink::Associativity::Left;

  auto pair = binop_table.Register(plus, p, a, ty, ty, ty, test_binop_table_fn);

  result &= Test(out, "BinopTable::Register", pair.first == plus);

  auto opt = binop_table.Lookup(plus);

  result &=
      Test(out, "BinopTable::Lookup", opt.has_value() && opt->first == plus);

  binop_table.Unregister(plus);

  auto opt1 = binop_table.Lookup(plus);

  result &= Test(out, "BinopTable::Unregister", opt1.has_value());

  return TestFooter(out, name, result);
}
