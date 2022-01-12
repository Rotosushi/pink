#include "llvm/Support/Host.h" // llvm::sys::getProcessTriple()
#include "llvm/Support/TargetRegistry.h" // llvm::TargetRegistry::lookupTarget();
#include "llvm/Support/TargetSelect.h"

#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetMachine.h"


#include "Test.hpp"
#include "ops/TestBinopCodegen.hpp"
#include "ops/BinopCodegen.hpp"

#include "type/IntType.hpp"

pink::Outcome<pink::Error, llvm::Value*> test_binop_codegen_fn(llvm::Value* left, llvm::Value* right, pink::Environment& env)
{
    std::string s("");
    pink::Error err(pink::Error::Kind::Syntax, s, pink::Location());
    return pink::Outcome<pink::Error, llvm::Value*>(err);
}



bool TestBinopCodegen(std::ostream& out)
{
    bool result = true;
    out << "\n-----------------------\n";
    out << "Testing pink::BinopCodegen: \n";

    pink::StringInterner symbols;
    pink::StringInterner operators;
    pink::TypeInterner   types;
    pink::SymbolTable    bindings;

    llvm::LLVMContext context;
    llvm::IRBuilder<> builder(context);

    llvm::InitializeAllTargetInfos();
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetDisassembler();


    std::string    target_triple = llvm::sys::getProcessTriple();

    std::string error;
    const llvm::Target* target = llvm::TargetRegistry::lookupTarget(target_triple, error);

    if (!target)
    {
        pink::FatalError(error.data(), __FILE__, __LINE__);
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


    pink::Environment env(symbols, operators, types, bindings,
        target_triple, data_layout, context, module, builder);

    pink::Type* ty = env.types.GetIntType();
    pink::BinopCodegen binop_codegen(ty, test_binop_codegen_fn);

    result &= Test(out, "BinopCodegen::result_type", binop_codegen.result_type == ty);

    result &= Test(out, "BinopCodegen::generate", binop_codegen.generate == test_binop_codegen_fn);


    result &= Test(out, "pink::BinopCodegen", result);
    out << "\n-----------------------\n";
    return result;
}