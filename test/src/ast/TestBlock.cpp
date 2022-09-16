
#include "Test.h"
#include "ast/TestBlock.h"

#include "ast/Bool.h"
#include "ast/Int.h"
#include "ast/Bind.h"
#include "ast/Binop.h"
#include "ast/Block.h"

#include "aux/Environment.h"


bool TestBlock(std::ostream& out)
{
	bool result = true;

  out << "\n-----------------------\n";
  out << "Testing pink::Block: \n";
  
  auto options = std::make_shared<pink::CLIOptions>();
  auto env = pink::NewGlobalEnv(options);   
  // "x := true; 3 + 5"
  pink::InternedString plus  = env->operators->Intern("+");
  pink::InternedString var_x = env->symbols->Intern("x");
  pink::Location l0(0, 0, 0, 1);
  pink::Location l1(0, 5, 0, 9);
  pink::Location l2(0, 0, 0, 6); // Location of the Bind
  pink::Location l3(0, 11, 0, 12);
  pink::Location l4(0, 15, 0, 16);
  pink::Location l5(0, 11, 0, 16); // Location of the Binop
  pink::Location l6(0, 0, 0, 16); // Location of the Block
  pink::Type*  int_t = env->types->GetIntType();
  //pink::Type*  bool_t = env.types.GetBoolType();
  
  std::unique_ptr<pink::Bool> bool0 = std::make_unique<pink::Bool>(l1, true);
  //pink::Ast* bool0_p = bool0.get();
  
  std::unique_ptr<pink::Bind> d0 = std::make_unique<pink::Bind>(l2, var_x, std::move(bool0));
  //pink::Ast* d0_p = d0.get();
  
  
  std::unique_ptr<pink::Int> i1 = std::make_unique<pink::Int>(l3, 3);
  //pink::Ast* i1_p = i1.get();
  
  std::unique_ptr<pink::Int> i2 = std::make_unique<pink::Int>(l4, 5);
  //pink::Ast* i2_p = i2.get();
  
  std::unique_ptr<pink::Binop> b0 = std::make_unique<pink::Binop>(l5, plus, std::move(i1), std::move(i2));
  //pink::Ast* b0_p = b0.get();
  
  std::vector<std::unique_ptr<pink::Ast>> stmts;
  stmts.emplace_back(std::move(d0));
  stmts.emplace_back(std::move(b0));
  
  std::unique_ptr<pink::Block> block = std::make_unique<pink::Block>(l6, stmts);

  result &= Test(out, "Block::getKind()", block->getKind() == pink::Ast::Kind::Block);

  result &= Test(out, "Block::GetLoc()", block->GetLoc() == l6);

  result &= Test(out, "Block::classof()", block->classof(block.get()));

  pink::Block::iterator iter = block->begin();

  result &= Test(out, "Block::iterator", iter != block->end());
  
  pink::Outcome<pink::Type*, pink::Error> getype_outcome(pink::Error(pink::Error::Code::None, pink::Location()));
  
  result &= Test(out, "Block::Getype()",
           (getype_outcome = block->Getype(*env))
        && (getype_outcome.GetOne() == int_t));
  
  result &= Test(out, "pink::Block", result);
  out << "\n-----------------------\n";
  return result;
}
