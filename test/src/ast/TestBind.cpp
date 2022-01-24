#include "Test.h"
#include "ast/TestBind.h"

#include "ast/Nil.h"
#include "ast/Bind.h"

#include "aux/Environment.h"

#include "llvm/Support/Host.h" // llvm::sys::getProcessTriple()
#include "llvm/Support/TargetRegistry.h" // llvm::TargetRegistry::lookupTarget();
#include "llvm/Support/TargetSelect.h"

#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetMachine.h"

bool TestBind(std::ostream& out)
{
    bool result = true;
    out << "\n-----------------------\n";
    out << "Testing pink::Bind: \n";
    
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

    pink::InternedString v = env.symbols.Intern("v");
    pink::Location l0(0, 0, 0, 5);
    pink::Location l1(0, 6, 0, 9);
    std::unique_ptr<pink::Nil> n0 = std::make_unique<pink::Nil>(l1);
    pink::Ast* n0_p = n0.get();
    
    std::unique_ptr<pink::Bind> b0 = std::make_unique<pink::Bind>(l0, v, std::move(n0));

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

    -) Bind::symbol == symbol;

    -) Bind::term != nullptr;

    -) Bind::ToString() == std::string(symbol) + " := " + term->ToString();

    */

    result &= Test(out, "Bind::GetKind()", b0->getKind() == pink::Ast::Kind::Bind);
    result &= Test(out, "Bind::classof()", b0->classof(b0.get()));

    pink::Location l2(l0);
    pink::Location bl(b0->GetLoc());
    result &= Test(out, "Bind::GetLoc()", bl == l2);

    result &= Test(out, "Bind::symbol", b0->symbol == v);
    result &= Test(out, "Bind::term", b0->term.get() == n0_p);

    std::string bind_str = std::string(v) + std::string(" := ") + n0_p->ToString();

    result &= Test(out, "Bind::ToString()", b0->ToString() == bind_str);
    
    pink::Outcome<pink::Type*, pink::Error> bind_type = b0->Getype(env);
    
    result &= Test(out, "Bind::Getype()", bind_type && bind_type.GetOne() == env.types.GetNilType());

    result &= Test(out, "pink::Bind", result);
    out << "\n-----------------------\n";
    return result;
}
