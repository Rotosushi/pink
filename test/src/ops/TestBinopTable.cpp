#include "Test.h"

#include "ast/Binop.h"

#include "ops/TestBinopTable.h"
#include "ops/BinopTable.h"

#include "aux/Environment.h"

pink::Outcome<llvm::Value*, pink::Error> test_binop_table_fn(llvm::Value* left, llvm::Value* right, std::shared_ptr<pink::Environment> env)
{
  std::string s("");
  pink::Error err(pink::Error::Kind::Syntax, s, pink::Location());
  return pink::Outcome<llvm::Value*, pink::Error>(err);
}

bool TestBinopTable(std::ostream& out)
{
  bool result = true;
  out << "\n-----------------------\n";
  out << "Testing pink::BinopTable: \n";
  
  auto options = std::make_shared<pink::CLIOptions>();
  auto env     = pink::NewGlobalEnv(options);

  pink::InternedString plus = env->operators->Intern("+");
  pink::Type* ty = env->types->GetIntType();
  pink::BinopTable binop_table;
  pink::Precedence p = 5;
  pink::Associativity a = pink::Associativity::Left;

  auto pair = binop_table.Register(plus, p, a, ty, ty, ty, test_binop_table_fn);

  result &= Test(out, "BinopTable::Register", pair.first == plus);

  auto opt = binop_table.Lookup(plus);

  result &= Test(out, "BinopTable::Lookup", opt.hasValue() && pair.first == plus);

  binop_table.Unregister(plus);

  auto opt1 = binop_table.Lookup(plus);

  result &= Test(out, "BinopTable::Unregister", !opt1.hasValue());

  result &= Test(out, "pink::BinopTable", result);
  out << "\n-----------------------\n";
  return result;
}
