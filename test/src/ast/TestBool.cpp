#include "Test.h"
#include "ast/TestBool.h"

#include "ast/Bool.h"

#include "aux/Environment.h"

#include "llvm/Support/Host.h" // llvm::sys::getProcessTriple()
#include "llvm/Support/TargetRegistry.h" // llvm::TargetRegistry::lookupTarget();
#include "llvm/Support/TargetSelect.h"

#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetMachine.h"



bool TestBool(std::ostream& out)
{
    bool result = true;
    result = true;

    out << "\n-----------------------\n";
    out << "Testing pink::Bool: \n";
    
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

    pink::Location l0(0, 3, 0, 9);
    std::unique_ptr<pink::Bool> b0 = std::make_unique<pink::Bool>(l0, true);
    pink::Type* bool_t = env.types.GetBoolType(); 

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

        -) Bool::ToString() == "Bool"
    */
    result &= Test(out, "Bool::GetKind()", b0->getKind() == pink::Ast::Kind::Bool);

    result &= Test(out, "Bool::classof()", b0->classof(b0.get()));

    pink::Location l1(l0);
    pink::Location bl(b0->GetLoc());
    result &= Test(out, "Bool::GetLoc()", bl == l1);

    result &= Test(out, "Bool::value", b0->value == true);

    result &= Test(out, "Bool::ToString()", b0->ToString() == "true");
    
    pink::Outcome<pink::Type*, pink::Error> bool_type = b0->Getype(env);
    
    result &= Test(out, "Bool::Getype()", bool_type && bool_type.GetOne() == bool_t);

    result &= Test(out, "pink::Result", result);
    out << "\n-----------------------\n";
    return result;
}
