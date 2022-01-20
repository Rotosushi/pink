#include "Test.h"
#include "aux/TestEnvironment.h"
#include "aux/Environment.h"
#include "aux/Error.h"

#include "ast/Nil.h"

#include "llvm/Support/Host.h" // llvm::sys::getProcessTriple()
#include "llvm/Support/TargetRegistry.h" // llvm::TargetRegistry::lookupTarget();
#include "llvm/Support/TargetSelect.h"

#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetMachine.h"

bool TestEnvironment(std::ostream& out)
{
    bool result = true;
    out << "\n-----------------------\n";
    out << "Testing Pink::Environment: \n";

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


    pink::InternedString symb = env.symbols.Intern("x");

    result &= Test(out, "Environment::symbols", symb == std::string("x"));


    pink::InternedString op = env.operators.Intern("+");

    result &= Test(out, "Environment::operators", op == std::string("+"));


    pink::Type* type = env.types.GetNilType();

    result &= Test(out, "Environment::types", type->ToString() == std::string("Nil"));

    pink::Location l(0, 7, 0, 10);
    llvm::Value* nil = env.ir_builder.getFalse();

    env.bindings.Bind(symb, type, nil);

    llvm::Optional<std::pair<pink::Type*, llvm::Value*>> term = env.bindings.Lookup(symb);

    // since they point to the same memory, nil, and the bound
    // term's pointer values compare equal if everything works.
    result &= Test(out, "Environment::bindings", term.hasValue() && (*term).first == type && (*term).second == nil);


    result &= Test(out, "Environment::target_triple", env.target_triple == target_triple);

    result &= Test(out, "Environment::data_layout", env.data_layout == data_layout);


    // there isn't a easy way of telling that the LLVMContext
    // that the environment was created around is the same as
    // the one constructed. if the environment took a pointer
    // instead of a reference, we could do pointer comparison,
    // but other than that, none of the methods associate
    // an individual context with a name or anything truly
    // unique. We could check that this module supports
    // Typed Pointers, but that isn't necessarily the same condition.

    result &= Test(out, "Environment::module", env.module.getModuleIdentifier() == std::string("TestEnvironment"));

    llvm::Type* ltype = llvm::Type::getInt8Ty(env.context);
    llvm::Type* irtype = env.ir_builder.getInt8Ty();

    result &= Test(out, "Environment::ir_builder", ltype == irtype);


    result &= Test(out, "pink::Environment", result);

    out << "\n-----------------------\n";
    return result;
}
