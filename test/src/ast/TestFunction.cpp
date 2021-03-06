#include "Test.h"
#include "ast/TestFunction.h"
#include "ast/Function.h"

#include "ast/Bool.h"
#include "ast/Int.h"
#include "ast/Variable.h"

#include "aux/Environment.h"


#include "llvm/Support/Host.h" // llvm::sys::getProcessTriple()
#include "llvm/Support/TargetRegistry.h" // llvm::TargetRegistry::lookupTarget();
#include "llvm/Support/TargetSelect.h"

#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetMachine.h"

bool TestFunction(std::ostream& out)
{
	bool result = true;
    out << "\n-----------------------\n";
    out << "Testing Pink::Function: \n";
    
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
    
    /*
    
    fn first(x: Int, y: Bool) 
    {
    	x;
    }
    
    fn second(x: Int, y: Bool) 
    {
    	y;
    }
    
    */
    
    pink::InternedString x = env.symbols.Intern("x"), y = env.symbols.Intern("y");
    pink::InternedString fname = env.symbols.Intern("first"), sname = env.symbols.Intern("second");
    pink::Type *int_t = env.types.GetIntType(), *bool_t = env.types.GetBoolType();
    pink::Location fl = {0,  0, 4, 34};
    pink::Location xl = {3, 30, 3, 31};
    pink::Location sl = {6, 36, 9, 70};
    pink::Location yl = {8, 64, 8, 65};
    
    std::unique_ptr<pink::Ast> xv = std::make_unique<pink::Variable>(xl, x);
    std::unique_ptr<pink::Ast> yv = std::make_unique<pink::Variable>(yl, y);
    
    std::vector<std::pair<pink::InternedString, pink::Type*>> fargs = {{x, int_t}, {y, bool_t}};
    std::vector<std::pair<pink::InternedString, pink::Type*>> sargs = {{x, int_t}, {y, bool_t}};
    
    pink::Type* ftype = env.types.GetFunctionType(int_t, {int_t, bool_t});
    pink::Type* stype = env.types.GetFunctionType(bool_t, {int_t, bool_t});
    
    std::unique_ptr<pink::Function> first = std::make_unique<pink::Function>(fl, fname, fargs, std::move(xv), &env.bindings);
    
    std::unique_ptr<pink::Function> second = std::make_unique<pink::Function>(sl, sname, sargs, std::move(yv), &env.bindings);
    
    result &= Test(out, "Function::getKind()", first->getKind() == pink::Ast::Kind::Function);
    
    result &= Test(out, "Function::GetLoc()", first->GetLoc() == fl);
    
    result &= Test(out, "Function::classof()", first->classof(first.get()));
    
    pink::Outcome<pink::Type*, pink::Error> fty_res = first->Getype(env);
    pink::Outcome<pink::Type*, pink::Error> sty_res = second->Getype(env);
    
    result &= Test(out, "Function::Getype()", 
    				   (fty_res) && (sty_res)
    				&& (fty_res.GetOne()->EqualTo(ftype))
    				&& (sty_res.GetOne()->EqualTo(stype)));
    
    
	result &= Test(out, "pink::Function", result);
    out << "\n-----------------------\n";
    return result;
}













