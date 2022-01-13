#include "llvm/Support/Host.h" // llvm::sys::getProcessTriple()
#include "llvm/Support/TargetRegistry.h" // llvm::TargetRegistry::lookupTarget();
#include "llvm/Support/TargetSelect.h"

#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetMachine.h"

#include "Test.hpp"
#include "ops/TestBinopLiteral.hpp"
#include "ops/BinopLiteral.hpp"

#include "aux/Environment.hpp"

pink::Outcome<llvm::Value*, pink::Error> test_binop_literal_fn(llvm::Value* left, llvm::Value* right, pink::Environment& env)
{
    std::string s("");
    pink::Error err(pink::Error::Kind::Syntax, s, pink::Location());
    return pink::Outcome<llvm::Value*, pink::Error>(err);
}

bool TestBinopLiteral(std::ostream& out)
{
    bool result = true;
    out << "\n-----------------------\n";
    out << "Testing pink::BinopLiteral: \n";

    pink::StringInterner symbols;
    pink::StringInterner operators;
    pink::TypeInterner   types;
    pink::SymbolTable    bindings;
    pink::BinopTable     binops;
    pink::UnopTable      unops;

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


    pink::Environment env(symbols, operators, types, bindings, binops, unops,
                          target_triple, data_layout, context, module, builder);

    pink::Type* ty = env.types.GetIntType();
    pink::Precedence p = 5;
    pink::Associativity a = pink::Associativity::Left;
    pink::BinopLiteral binop(p, a);

    result &= Test(out, "BinopLiteral::precedence", binop.precedence == p);
    result &= Test(out, "BinopLiteral::associativity", binop.associativity == a);

    auto pair = binop.Register(ty, ty, ty, test_binop_literal_fn);

    result &= Test(out, "BinopLiteral::Register()", pair.first == std::make_pair(ty, ty));

    auto opt = binop.Lookup(ty, ty);

    result &= Test(out, "BinopLiteral::Lookup()", opt.hasValue() && (*opt).first == std::make_pair(ty, ty));

    binop.Unregister(ty, ty);

    opt = binop.Lookup(ty, ty);

    result &= Test(out, "BinopLiteral::Unregister()", !opt.hasValue());

    result &= Test(out, "pink::BinopLiteral", result);
    out << "\n-----------------------\n";
    return result;
}
