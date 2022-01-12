#include "llvm/Support/Host.h" // llvm::sys::getProcessTriple()
#include "llvm/Support/TargetRegistry.h" // llvm::TargetRegistry::lookupTarget();
#include "llvm/Support/TargetSelect.h"

#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetMachine.h"

#include "Test.hpp"
#include "aux/TestBinopTable.hpp"
#include "aux/BinopTable.hpp"

pink::Outcome<pink::Error, llvm::Value*> test_binop_table_fn(llvm::Value* left, llvm::Value* right, pink::Environment& env)
{
    std::string s("");
    pink::Error err(pink::Error::Kind::Syntax, s, pink::Location());
    return pink::Outcome<pink::Error, llvm::Value*>(err);
}

bool TestBinopTable(std::ostream& out)
{
    bool result = true;
    out << "\n-----------------------\n";
    out << "Testing pink::BinopTable: \n";

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

    pink::InternedString plus = env.operators.Intern("+");
    pink::Type* ty = env.types.GetIntType();
    pink::BinopTable binop_table;
    pink::Precedence p = 5;
    pink::Associativity a = pink::Associativity::Left;

    auto pair = binop_table.Register(plus, p, a, ty, ty, ty, test_binop_table_fn);

    result &= Test(out, "BinopTable::Register", pair.first == plus);

    auto opt = binop_table.Lookup(plus);

    result &= Test(out, "BinopTable::Lookup", opt.hasValue() && pair.first == plus);

    binop_table.Unregister(plus);

    auto opt1 = binop_table.Lookup(plus);

    result &= Test(out, "BinopTable::Unregister", !opt1.hasValue());

    result &= Test(out, "pink::BinopTable", result);
    out << "\n-----------------------\n";
    return result;
}
