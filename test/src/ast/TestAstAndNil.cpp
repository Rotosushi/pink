#include <string>
#include <sstream>

#include "Test.h"
#include "ast/TestAstAndNil.h"
#include "ast/Nil.h"

#include "aux/Environment.h"

#include "llvm/Support/Host.h" // llvm::sys::getProcessTriple()
#include "llvm/Support/TargetRegistry.h" // llvm::TargetRegistry::lookupTarget();
#include "llvm/Support/TargetSelect.h"

#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetMachine.h"


bool TestAstAndNil(std::ostream& out)
{
    bool result = true;
    out << "\n-----------------------\n";
    out << "Testing pink::Ast and pink::Nil: \n";
    
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

        -) Nil::ToString() == "nil"

    */

	pink::Type* nil_t = env.types.GetNilType();
    pink::Location l0(0, 14, 0, 15);
    std::unique_ptr<pink::Nil> a = std::make_unique<pink::Nil>(l0);

    result &= Test(out, "Nil::GetKind()", a->getKind() == pink::Ast::Kind::Nil);
    result &= Test(out, "Nil::classof()", a->classof(a.get()));

    pink::Location l1(l0);

    result &= Test(out, "Nil::GetLoc()", l0 == l1);

    std::string nil_str = "nil";

    result &= Test(out, "Nil::ToString()", a->ToString() == nil_str);
    
    pink::Outcome<pink::Type*, pink::Error> nil_type = a->Getype(env);
    
    result &= Test(out, "Nil::Getype()", nil_type && nil_type.GetOne() == nil_t);

    result &= Test(out, "pink::Nil", result);
    out << "\n-----------------------\n";
    return result;
}
