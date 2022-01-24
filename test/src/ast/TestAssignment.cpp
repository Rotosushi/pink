#include <memory>

#include "Test.h"
#include "ast/TestAssignment.h"
#include "ast/Assignment.h"
#include "ast/Variable.h"
#include "ast/Nil.h"

#include "aux/Environment.h"

#include "llvm/Support/Host.h" // llvm::sys::getProcessTriple()
#include "llvm/Support/TargetRegistry.h" // llvm::TargetRegistry::lookupTarget();
#include "llvm/Support/TargetSelect.h"

#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetMachine.h"

bool TestAssignment(std::ostream& out)
{
	bool result = true;
	out << "\n-----------------------\n";
    out << "Testing pink::Assignment: \n";
    
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
        return false; // this suppresses a linter warning from clang-tidy 
        			  // even though FatalError() calls exit(1), and so 
        			  // this return statement is unreachable code.
        			  // TODO: figure out how to mark FatalError as a 
        			  // program endpoint.
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


    pink::Environment env(symbols, operators, types, bindings, binops, unops,
                          target_triple, data_layout, context, module, builder);
                          
    /*
    	An Assignment term looks like 
    	
    	<assignable-term> '=' <value-term>
    	e.x. "x = 7" or "y = nil"
    */
    
    pink::InternedString symb = env.symbols.Intern("z");
    pink::Location ll(0, 0, 0, 1);
    pink::Location rl(0, 4, 0, 7);
    pink::Location al(0, 0, 0, 7);
    std::unique_ptr<pink::Ast> var = std::make_unique<pink::Variable>(ll, symb);
    pink::Ast* var_p = var.get();
	std::unique_ptr<pink::Ast> nil = std::make_unique<pink::Nil>(rl);
	pink::Ast* nil_p = nil.get();
	
	llvm::Value* nil_v = env.ir_builder.getFalse();
    pink::Type*  nil_t = env.types.GetNilType();
    
    env.bindings.Bind(symb, nil_t, nil_v);
	
	std::unique_ptr<pink::Assignment> ass = std::make_unique<pink::Assignment>(al, std::move(var), std::move(nil));
	
	result &= Test(out, "Assignment::getKind()", ass->getKind() == pink::Ast::Kind::Assignment);
	
	result &= Test(out, "Assignment::classof()", ass->classof(ass.get()));
	
	result &= Test(out, "Assignment::GetLoc()", ass->GetLoc() == al);
	
	result &= Test(out, "Assignment::left", ass->left.get() == var_p);
	
	result &= Test(out, "Assignment::right", ass->right.get() == nil_p);
	
	std::string ass_str = var_p->ToString() + " = " + nil_p->ToString();
	
	result &= Test(out, "Assignment::ToString()", ass->ToString() == ass_str);
	
	pink::Outcome<pink::Type*, pink::Error> ass_type = ass->Getype(env);
	
	result &= Test(out, "Assignment::Getype()", ass_type && ass_type.GetOne() == nil_t);
	
	result &= Test(out, "pink::Assignment", result);
    out << "\n-----------------------\n";
	return result;
}



