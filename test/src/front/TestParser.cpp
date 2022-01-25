
#include "llvm/Support/Host.h" // llvm::sys::getProcessTriple()
#include "llvm/Support/TargetRegistry.h" // llvm::TargetRegistry::lookupTarget();
#include "llvm/Support/TargetSelect.h"

#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetMachine.h"

#include "Test.h"
#include "front/TestParser.h"
#include "front/Parser.h"

#include "aux/Environment.h"

#include "ast/Nil.h"
#include "ast/Int.h"
#include "ast/Bool.h"
#include "ast/Variable.h"
#include "ast/Assignment.h"
#include "ast/Bind.h"
#include "ast/Binop.h"
#include "ast/Unop.h"

#include "kernel/UnopPrimitives.h"
#include "kernel/BinopPrimitives.h"


bool TestParser(std::ostream& out)
{
	bool result = true;
    out << "\n-----------------------\n";
    out << "Testing Pink::Parser: \n";
    
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
    	Parser Tests
    	
    		Parses each Basic lexeme 
    		Parses Binding and Assignment
    		Parses Unop expressions
    		Parser Binop expressions
    			Parses Basic Binop Expressions (single operator)
    			Parses Complex Binop Expressions 
    				(multiple operators and up/down + down/up precedence changes)
    		Parses Parenthesized expressions
    */       
    
    pink::Outcome<std::unique_ptr<pink::Ast>, pink::Error> parser_result(parser.Parse("nil", env));
    
    pink::Ast* term = nullptr;
    
    result &= Test(out, "Parser::Parse(Nil)", 
    	(parser_result) && (term = parser_result.GetOne().get()) && (llvm::isa<pink::Nil>(term)));
                          
    parser_result = parser.Parse("10", env);
    
    result &= Test(out, "Parser::Parse(Int)", 
    	(parser_result) && (term = parser_result.GetOne().get()) && (llvm::isa<pink::Int>(term)));
    	
    parser_result = parser.Parse("true", env);
    
    result &= Test(out, "Parser::Parse(Bool)", 
    	(parser_result) && (term = parser_result.GetOne().get()) && (llvm::isa<pink::Bool>(term)));
    
    parser_result = parser.Parse("x", env);
    
    result &= Test(out, "Parser::Parse(Variable)", 
    	(parser_result) && (term = parser_result.GetOne().get()) && (llvm::isa<pink::Variable>(term)));
    
    parser_result = parser.Parse("x := 1", env);
    
    result &= Test(out, "Parser::Parse(Bind)", 
    	(parser_result) && (term = parser_result.GetOne().get()) && (llvm::isa<pink::Bind>(term)));
    
    parser_result = parser.Parse("x = 2", env);
    
    result &= Test(out, "Parser::Parse(Assignment)", 
    	(parser_result) && (term = parser_result.GetOne().get()) && (llvm::isa<pink::Assignment>(term)));
    
    parser_result = parser.Parse("!true", env);
    
    result &= Test(out, "Parser::Parse(unary expression)", 
    	(parser_result) && (term = parser_result.GetOne().get()) && (llvm::isa<pink::Unop>(term)));
    	
    parser_result = parser.Parse("1 + 1", env);
    
    result &= Test(out, "Parser::Parse(binary expression)", 
    	(parser_result) && (term = parser_result.GetOne().get()) && (llvm::isa<pink::Binop>(term)));
    	
    parser_result = parser.Parse("6 + 3 * 4 == 3 * 2 + 12", env);
    
    result &= Test(out, "Parser::Parse(complex binary expression)",
    	(parser_result) && (term = parser_result.GetOne().get()) && (llvm::isa<pink::Binop>(term)));
    
    parser_result = parser.Parse("(1 + 1) - (1 + 1)", env);
    
    result &= Test(out, "Parser::Parse(parenthesized expression)", 
    	(parser_result) && (term = parser_result.GetOne().get()) && (llvm::isa<pink::Binop>(term)));
    
    result &= Test(out, "pink::Parser", result);
    out << "\n-----------------------\n";
    return result;
}



