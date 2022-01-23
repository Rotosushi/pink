#include "Test.h"
#include "ast/TestInt.h"

#include "ast/Int.h"

#include "aux/Environment.h"

#include "llvm/Support/Host.h" // llvm::sys::getProcessTriple()
#include "llvm/Support/TargetRegistry.h" // llvm::TargetRegistry::lookupTarget();
#include "llvm/Support/TargetSelect.h"

#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetMachine.h"


bool TestInt(std::ostream& out)
{
    int result = true;
    result = true;

    out << "\n-----------------------\n";
    out << "Testing pink::Int: \n";
    
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

	pink::Type* int_t = env.types.GetIntType();
    pink::Location l0(0, 3, 0, 7);
    pink::Int* i0 = new pink::Int(l0, 42);

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

        -) Int::ToString() == "42"
    */
    result &= Test(out, "Int::GetKind()", i0->getKind() == pink::Ast::Kind::Int);

    result &= Test(out, "Int::classof()", i0->classof(i0));

    pink::Location l1(l0);
    pink::Location il(i0->GetLoc());
    result &= Test(out, "Int::GetLoc()", il == l1);

    result &= Test(out, "Int::value", i0->value == 42);

    result &= Test(out, "Int::ToString()", i0->ToString() == "42");
    
    pink::Outcome<pink::Type*, pink::Error> int_type = i0->Getype(env);
    
    result &= Test(out, "Int::Getype()", int_type && int_type.GetOne() == int_t);

    delete i0;

    result &= Test(out, "pink::Int", result);
    out << "\n-----------------------\n";
    return result;
}
