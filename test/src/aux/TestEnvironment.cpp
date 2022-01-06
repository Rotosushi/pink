
#include "aux/TestEnvironment.hpp"
#include "aux/Environment.hpp"
#include "aux/Error.hpp"

#include "llvm/Support/Host.h" // llvm::sys::getProcessTriple()

#include "llvm/MC/TargetRegistry.h" // llvm::TargetRegistry::lookupTarget();


bool TestEnvironment(std::ostream& out)
{
    bool result = true;
    out << "\n-----------------------\n";
    out << "Testing Pink::StringInterner: \n";

    pink::StringInterner symbols;
    pink::StringInterner operators;
    pink::TypeInterner   types;
    pink::SymbolTable    bindings;

    llvm::LLVMContext context;
    llvm::IRBuilder<> builder(context);

    std::string    target_triple = llvm::sys::getProcessTriple();

    std::string error;
    llvm::Target* target = llvm::TargetRegistry::lookupTarget(target_triple, error);

    if (!target)
    {
        FatalError(error, __FILE__, __LINE__);
    }

    llvm::DataLayout data_layout;
    llvm::Module      module;


    pink::Environment env(symbols, operators, types, bindings,
        target_triple, data_layout, context, module, builder);


    if (result)
        out << "Test: pink::StringInterner: Passed\n";
    else
        out << "Test: pink::StringInterner: Failed\n";

    out << "\n-----------------------\n";
    return result;
}
