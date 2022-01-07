
#include "aux/TestEnvironment.hpp"
#include "aux/Environment.hpp"
#include "aux/Error.hpp"

#include "ast/Nil.hpp"

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

    llvm::LLVMContext context;
    llvm::IRBuilder<> builder(context);

    llvm::InitializeAllTargetInfos();
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


    pink::Environment env(symbols, operators, types, bindings,
        target_triple, data_layout, context, module, builder);


    pink::InternedString symb = env.symbols.Intern("x");

    if (symb == std::string("x"))
    {
        out << "\tTest: Environment::symbols: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Environment::symbols: Failed\n";
    }


    pink::InternedString op = env.operators.Intern("+");

    if (op == std::string("+"))
    {
        out << "\tTest: Environment::operators: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Environment::operators: Failed\n";
    }


    pink::Type* type = env.types.GetNilType();

    if (type->ToString() == std::string("Nil"))
    {
        out << "\tTest: Environment::types: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Environment::types: Failed\n";
    }

    pink::Location l(0, 7, 0, 10);
    pink::Nil* nil = new pink::Nil(l);

    env.bindings.Bind(symb, nil);

    llvm::Optional<pink::Ast*> term = env.bindings.Lookup(symb);

    // since they point to the same memory, nil, and the bound
    // term's pointer values compare equal if everything works.
    if (term.hasValue() && *term == nil)
    {
        out << "\tTest: Environment::bindings: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Environment::bindings: Failed\n";
    }


    if (env.target_triple == target_triple)
    {
        out << "\tTest: Environment::target_triple: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Environment::target_triple: Failed\n";
    }

    if (env.data_layout == data_layout)
    {
        out << "\tTest: Environment::data_layout: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest:: Environment::data_layout: Failed\n";
    }


    // there isn't a easy way of telling that the LLVMContext
    // that the environment was created around is the same as
    // the one constructed. if the environment took a pointer
    // instead of a reference, we could do pointer comparison,
    // but other than that, none of the methods associate
    // an individual context with a name or anything truly
    // unique. We could check that this module supports
    // Typed Pointers, but that isn't necessarily the same condition.


    if (env.module.getModuleIdentifier() == std::string("TestEnvironment"))
    {
        out << "\tTest: Environment::module: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest:: Environment::module: Failed\n";
    }

    llvm::Type* ltype = llvm::Type::getInt8Ty(env.context);
    llvm::Type* irtype = env.ir_builder.getInt8Ty();

    if (ltype == irtype)
    {
        out << "\tTest: Environment::ir_builder: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Environment::ir_builder: Failed\n";
    }


    if (result)
        out << "Test: pink::Environment: Passed\n";
    else
        out << "Test: pink::Environment: Failed\n";

    out << "\n-----------------------\n";
    return result;
}
