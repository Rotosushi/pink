
#include "Test.h"
#include "kernel/TestFirstPhase.h"
#include "aux/Environment.h"

#include "ast/Nil.h"
#include "ast/Int.h"

#include "ast/Bool.h"
#include "ast/Variable.h"
#include "ast/Assignment.h"
#include "ast/Bind.h"
#include "ast/Binop.h"
#include "ast/Unop.h"
#include "ast/Block.h"

#include "kernel/UnopPrimitives.h"
#include "kernel/BinopPrimitives.h"

#include "llvm/Support/Host.h" // llvm::sys::getProcessTriple()
#include "llvm/Support/TargetRegistry.h" // llvm::TargetRegistry::lookupTarget();
#include "llvm/Support/TargetSelect.h"

#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetMachine.h"


bool TestFirstPhase(std::ostream& out)
{
	bool result = true;
    out << "\n-----------------------\n";
    out << "Testing Pink First Phase: \n";
    
    pink::Parser         parser;
    pink::StringInterner symbols;
    pink::StringInterner operators;
    pink::TypeInterner   types;
    pink::SymbolTable    bindings;
    pink::BinopTable     binops;
    pink::UnopTable      unops;

    llvm::LLVMContext context;
    llvm::IRBuilder<> builder(context);

    //llvm::InitializeAllTargetInfos();
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetDisassembler();


    std::string target_triple = llvm::sys::getProcessTriple();

    std::string error;
    const llvm::Target* target = llvm::TargetRegistry::lookupTarget(target_triple, error);

    if (!target)
    {
        pink::FatalError(error.data(), __FILE__, __LINE__);
        return false;
    }

    std::string cpu = "x86-64";
    std::string cpu_features = "";
    llvm::TargetOptions target_options;
    llvm::Reloc::Model crm = llvm::Reloc::Model::PIC_;
    llvm::CodeModel::Model code_model = llvm::CodeModel::Model::Small;
    llvm::CodeGenOpt::Level opt_level = llvm::CodeGenOpt::Level::None;

    llvm::TargetMachine* target_machine = target->createTargetMachine(target_triple,
                                                                      cpu,
                                                                      cpu_features,
                                                                      target_options,
                                                                      crm,
                                                                      code_model,
                                                                      opt_level);

    llvm::DataLayout data_layout(target_machine->createDataLayout());
    llvm::Module      module("TestEnvironment", context);


    pink::Environment env(parser, symbols, operators, types, bindings, binops, unops,
                          target_triple, data_layout, context, module, builder);
    pink::InitializeUnopPrimitives(env);
    pink::InitializeBinopPrimitives(env);

	/*
		Parser emits correct Ast for each Ast node kind,
		and each Ast emits the right type after being returned.
		
		Parser successfully emits errors when it encounters 
		invalid syntax.
		
		Getype successfully emits errors when it encounters 
		untypeable expressions.
	*/
	pink::Ast* term = nullptr;
	pink::Block* block = nullptr;
	pink::Block::iterator iter;
	pink::Outcome<pink::Type*, pink::Error> getype_result = pink::Error(pink::Error::Kind::Default,
															"Default Error",
															pink::Location());
	
	
	// test parsing and typing a nil literal
	pink::Outcome<std::unique_ptr<pink::Ast>, pink::Error> parser_result = env.parser.Parse("nil", env);
	
	result &= Test(out, "Parser::Parse(nil), Getype(Nil)", 
					   (parser_result) 
				    && ((term  = parser_result.GetOne().get()) != nullptr)
    				&& ((block = llvm::dyn_cast<pink::Block>(term)) != nullptr)
    				&& ((iter  = block->begin()) != block->end())
				    && llvm::isa<pink::Nil>(iter->get())
				    && (getype_result = term->Getype(env))
				    && (getype_result.GetOne() == env.types.GetNilType()));
		
		
	// test parsing and typing a integer literal
	parser_result = env.parser.Parse("108", env);
	
	result &= Test(out, "Parser::Parse(108), Getype(Int)", 
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
    			   && ((block = llvm::dyn_cast<pink::Block>(term)) != nullptr)
    			   && ((iter  = block->begin()) != block->end())
				   && llvm::isa<pink::Int>(iter->get())
				   && (getype_result = term->Getype(env))
				   && (getype_result.GetOne() == env.types.GetIntType()));
			
			
	// test parsing and typing a boolean literal
	parser_result = env.parser.Parse("false", env);
	
	result &= Test(out, "Parser::Parse(false), Getype(Bool)", 
					 (parser_result) 
				  && ((term  = parser_result.GetOne().get()) != nullptr)
    			  && ((block = llvm::dyn_cast<pink::Block>(term)) != nullptr)
    			  && ((iter  = block->begin()) != block->end())
				  && llvm::isa<pink::Bool>(iter->get())
				  && (getype_result = term->Getype(env))
			      && (getype_result.GetOne() == env.types.GetBoolType()));
	
	
	// test parsing and typing a bound variable
	pink::InternedString x_var  = env.symbols.Intern("x");
	pink::Type*          x_type = env.types.GetBoolType();
	llvm::Value*         x_val  = env.ir_builder.getTrue();
	   
	env.bindings.Bind(x_var, x_type, x_val);
	parser_result = env.parser.Parse("x", env);
	
	result &= Test(out, "Parser::Parse(Variable), Getype(Variable, Bound)", 
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
    			   && ((block = llvm::dyn_cast<pink::Block>(term)) != nullptr)
    			   && ((iter  = block->begin()) != block->end())
				   && llvm::isa<pink::Variable>(iter->get())
				   && (getype_result = term->Getype(env))
				   && (getype_result.GetOne() == env.types.GetBoolType()));
				   
				   
	// test parsing a variable, and typing an unbound variable
	parser_result = env.parser.Parse("omega", env);
	
	result &= Test(out, "Parser::Parse(Variable), Getype::(Variable, unbound)", 
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
    			   && ((block = llvm::dyn_cast<pink::Block>(term)) != nullptr)
    			   && ((iter  = block->begin()) != block->end())
				   && llvm::isa<pink::Variable>(iter->get())
				   && !(getype_result = term->Getype(env)));
	
	
	
	// test parsing a bind expression, binding to a new variable
	parser_result = env.parser.Parse("y := true", env);
	
	result &= Test(out, "Parser::Parse(Bind), Getype(Bind, binding a new variable)", 
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
    			   && ((block = llvm::dyn_cast<pink::Block>(term)) != nullptr)
    			   && ((iter  = block->begin()) != block->end()) 
				   && llvm::isa<pink::Bind>(iter->get())
				   && (getype_result = term->Getype(env))
				   && (getype_result.GetOne() == env.types.GetBoolType()));
				   
	// test parsing a bind expression, binding to a already bound variable
	parser_result = env.parser.Parse("x := 123", env);
	
	result &= Test(out, "Parser::Parse(Bind), Getype(Bind, binding an existing variable)", 
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
    			   && ((block = llvm::dyn_cast<pink::Block>(term)) != nullptr)
    			   && ((iter  = block->begin()) != block->end())
				   && llvm::isa<pink::Bind>(iter->get())
				   && !(getype_result = term->Getype(env)));
				   
				   
	// test parsing an assignment expression, using the definition of 'x' above.
	parser_result = env.parser.Parse("x = false", env);
	
	result &= Test(out, "Parser::Parse(Assignment), Getype(Assignment, bound variable)", 
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
    			   && ((block = llvm::dyn_cast<pink::Block>(term)) != nullptr)
    			   && ((iter  = block->begin()) != block->end())
				   && llvm::isa<pink::Assignment>(iter->get())
				   && (getype_result = term->Getype(env))
				   && (getype_result.GetOne() == env.types.GetBoolType()));
				   
				   
	// test parsing an assignment expression, using an unbound variable
	parser_result = env.parser.Parse("omega = 49", env);
	
	result &= Test(out, "Parser::Parse(Assignment), Getype(Assignment, unbound variable)", 
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
    			   && ((block = llvm::dyn_cast<pink::Block>(term)) != nullptr)
    			   && ((iter  = block->begin()) != block->end())
				   && llvm::isa<pink::Assignment>(iter->get())
				   && !(getype_result = term->Getype(env)));
				   
				   
				   
	// test parsing a unop expression which is known, and is provided the correct type
	parser_result = env.parser.Parse("!x", env);
	
	result &= Test(out, "Parser::Parse(Unop), Getype(Unop, Known Unop, Valid Type)", 
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
    			   && ((block = llvm::dyn_cast<pink::Block>(term)) != nullptr)
    			   && ((iter  = block->begin()) != block->end()) 
				   && llvm::isa<pink::Unop>(iter->get())
				   && (getype_result = term->Getype(env))
				   && (getype_result.GetOne() == env.types.GetBoolType()));
				   
	// test parsing a unop expression which is known, and is provided the incorrect type
	parser_result = env.parser.Parse("!100", env);
	
	result &= Test(out, "Parser::Parse(Unop), Getype(Unop, Known Unop, invalid Type)", 
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
    			   && ((block = llvm::dyn_cast<pink::Block>(term)) != nullptr)
    			   && ((iter  = block->begin()) != block->end())
				   && llvm::isa<pink::Unop>(iter->get())
				   && !(getype_result = term->Getype(env)));
				   
	// test parsing a unop expression which is unknown
	parser_result = env.parser.Parse("@100", env);
	
	result &= Test(out, "Parser::Parse(Unop), Getype(Unop, Unknown Unop)", 
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
    			   && ((block = llvm::dyn_cast<pink::Block>(term)) != nullptr)
    			   && ((iter  = block->begin()) != block->end())
				   && llvm::isa<pink::Unop>(iter->get())
				   && !(getype_result = term->Getype(env)));
				   
				   
	// test parsing a binop expression which is known, and is provided the correct types
	parser_result = env.parser.Parse("x || !x", env); // a tautology
	
	result &= Test(out, "Parser::Parse(Binop), Getype(Binop, Known Binop, Valid Types)", 
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
    			   && ((block = llvm::dyn_cast<pink::Block>(term)) != nullptr)
    			   && ((iter  = block->begin()) != block->end())
				   && llvm::isa<pink::Binop>(iter->get())
				   && (getype_result = term->Getype(env))
				   && (getype_result.GetOne() == env.types.GetBoolType()));
				   
				   
	// test parsing a binop expression which is known, and is provided the incorrect types
	parser_result = env.parser.Parse("x + x", env);
	
	result &= Test(out, "Parser::Parse(Binop), Getype(Binop, Known Binop, Invalid Types)", 
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
    			   && ((block = llvm::dyn_cast<pink::Block>(term)) != nullptr)
    			   && ((iter  = block->begin()) != block->end())
				   && llvm::isa<pink::Binop>(iter->get())
				   && !(getype_result = term->Getype(env)));
				   
				   
	// test parsing a binop expression which is unknown, and is provided the incorrect types
	parser_result = env.parser.Parse("x $ x", env);
	
	result &= Test(out, "Parser::Parse(Binop), Getype(Binop, Unknown Binop)", !(parser_result));
				   
	
	// test parsing a complex binop expression which is known, and is provided the correct types
	parser_result = env.parser.Parse("(x || !x) && (x && !x)", env); // a contradiction
	
	result &= Test(out, "Parser::Parse(complex Binop), Getype(complex Binop, Known Binops, Valid Types)", 
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
    			   && ((block = llvm::dyn_cast<pink::Block>(term)) != nullptr)
    			   && ((iter  = block->begin()) != block->end())
				   && llvm::isa<pink::Binop>(iter->get())
				   && (getype_result = term->Getype(env))
				   && (getype_result.GetOne() == env.types.GetBoolType()));
				   
				   
	// test parsing a complex binop expression which is known, and is provided the incorrect types
	parser_result = env.parser.Parse("(x || !x) && (5 && 10)", env);
	
	result &= Test(out, "Parser::Parse(complex Binop), Getype(complex Binop, Known Binops, Invalid Types)",
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
    			   && ((block = llvm::dyn_cast<pink::Block>(term)) != nullptr)
    			   && ((iter  = block->begin()) != block->end())
				   && llvm::isa<pink::Binop>(iter->get())
				   && !(getype_result = term->Getype(env)));
				   
	// test parsing a complex binop expression which is unknown, and is provided the incorrect types
	parser_result = env.parser.Parse("(x || !x) $$ (x && !x)", env);
	
	result &= Test(out, "Parser::Parse(complex Binop), Getype(complex Binop, unknown Binops)", !(parser_result));
	
	
	// test parsing a mixed-type complex binop expression which is known, and is provided the correct types
	parser_result = env.parser.Parse("100 * 3 == 25 * 12", env);
	
	result &= Test(out, "Parser::Parse(mixed-type complex Binop), Getype(mixed-type complex Binop, Known Binops, Valid Types)", 
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
    			   && ((block = llvm::dyn_cast<pink::Block>(term)) != nullptr)
    			   && ((iter  = block->begin()) != block->end())
				   && llvm::isa<pink::Binop>(iter->get())
				   && (getype_result = term->Getype(env))
				   && (getype_result.GetOne() == env.types.GetBoolType()));
				   
	// test parsing a mixed-type complex binop expression which is known, and is provided the incorrect types
	parser_result = env.parser.Parse("100 * 3 == true * 12", env);
	
	result &= Test(out, "Parser::Parse(mixed-type complex Binop), Getype(mixed-type complex Binop, Known Binops, Invalid Types)", 
					  (parser_result) 
				   && ((term  = parser_result.GetOne().get()) != nullptr)
    			   && ((block = llvm::dyn_cast<pink::Block>(term)) != nullptr)
    			   && ((iter  = block->begin()) != block->end()) 
				   && llvm::isa<pink::Binop>(iter->get())
				   && !(getype_result = term->Getype(env)));
				   
	// test parsing a complex binop expression which is unknown, and is provided the incorrect types
	parser_result = env.parser.Parse("100 * 3 == 25 $$ 12", env);
	
	result &= Test(out, "Parser::Parse(mixed-type complex Binop), Getype(mixed-type complex Binop, unknown Binops)", !(parser_result));
	
	
	parser_result = env.parser.Parse("x = false; 1 + 1", env);
	
	result &= Test(out, "Parser::Parse(Block), Getype(Block type is type of the last statement in the Block)",
					   (parser_result)
					&& ((term  = parser_result.GetOne().get()) != nullptr)
					&& (getype_result = term->Getype(env))
					&& (getype_result.GetOne() == env.types.GetIntType()));

	result &= Test(out, "pink First Phase", result);

    out << "\n-----------------------\n";
    return result;
}
