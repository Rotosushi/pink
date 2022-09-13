
#include "Test.h"
#include "ops/TestUnopTable.h"
#include "ops/UnopTable.h"

#include "aux/Environment.h"

#include "type/IntType.h"

pink::Outcome<llvm::Value*, pink::Error> test_table_fn(llvm::Value* term, std::shared_ptr<pink::Environment> env)
{
    pink::Error err(pink::Error::Code::None, pink::Location());
    if (!term || !env)
      return pink::Outcome<llvm::Value*, pink::Error>(err);
    else
      return pink::Outcome<llvm::Value*, pink::Error>(err);
}


bool TestUnopTable(std::ostream& out)
{
  bool result = true;
  out << "\n-----------------------\n";
  out << "Testing pink::UnopTable: \n";

  auto options = std::make_shared<pink::CLIOptions>();
  auto env     = pink::NewGlobalEnv(options);

  pink::InternedString minus = env->operators->Intern("-");
  pink::Type* ty = env->types->GetIntType();
  pink::UnopTable unop_table;

  auto pair = unop_table.Register(minus, ty, ty, test_table_fn);

  result &= Test(out, "UnopTable::Register", pair.first == minus);

  auto opt = unop_table.Lookup(minus);

  result &= Test(out, "UnopTable::Lookup", opt.hasValue() && pair.first == minus);

  unop_table.Unregister(minus);

  auto opt1 = unop_table.Lookup(minus);

  result &= Test(out, "UnopTable::Unregister", !opt1.hasValue());


  result &= Test(out, "pink::UnopTable", result);
  out << "\n-----------------------\n";
  return result;
}
