#include "Test.h"
#include "ast/TestUnop.h"
#include "ast/Unop.h"
#include "ast/Int.h"

#include "aux/Environment.h"

#include "kernel/UnopPrimitives.h"

#include "llvm/Support/Host.h" // llvm::sys::getProcessTriple()
#include "llvm/Support/TargetRegistry.h" // llvm::TargetRegistry::lookupTarget();
#include "llvm/Support/TargetSelect.h"

#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetMachine.h"

bool TestUnop(std::ostream& out)
{
    bool result = true;
    result = true;

    out << "\n-----------------------\n";
    out << "Testing pink::Unop: \n";
    
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


    pink::Environment env(symbols, operators, types, bindings, binops, unops,
                          target_triple, data_layout, context, module, builder);
                          
    pink::InitializeUnopPrimitives(env);

    pink::InternedString minus = env.operators.Intern("-");
    pink::Location l0(0, 0, 0, 1);
    pink::Location l1(0, 2, 0, 3);
    pink::Location l3(0, 0, 0, 3);
    pink::Type* int_t = env.types.GetIntType();
    pink::Int*  i0 = new pink::Int(l1, 1);
    pink::Unop* u0 = new pink::Unop(l3, minus, i0);

    /*
    The Ast class itself only provides a small
    amount of the functionality of any given
    syntax node.

    tests:

    -) An Ast node constructed with a particular kind
        holds that particular kind.

    -) classof() meets specifications:
        https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html#the-contract-of-classof

    -) An Ast node constructed with a particular Location
        holds that particular Location.

    -) Unop::symbol == symbol;

    -) Unop::right != nullptr;

    -) Unop::ToString() == std::string(op) + i0->ToString();

    */
    result &= Test(out, "Unop::GetKind()", u0->getKind() == pink::Ast::Kind::Unop);

    result &= Test(out, "Unop::classof()", u0->classof(u0));

    pink::Location ul(u0->GetLoc());
    result &= Test(out, "Unop::GetLoc()", ul == l3);

    result &= Test(out, "Unop::symbol", u0->op == minus);

    result &= Test(out, "Unop::term", u0-> right == i0);

    std::string unop_str = std::string(minus) + i0->ToString();

    result &= Test(out, "Unop::ToString()", u0->ToString() == unop_str);
    
    pink::Outcome<pink::Type*, pink::Error> unop_type = u0->Getype(env);
    
    result &= Test(out, "Unop::Getype()", unop_type && unop_type.GetOne() == int_t);

    delete u0;

    result &= Test(out, "pink::Unop", result);
    out << "\n-----------------------\n";
    return result;
}
