#include "llvm/Support/Host.h" // llvm::sys::getProcessTriple()
#include "llvm/Support/TargetRegistry.h" // llvm::TargetRegistry::lookupTarget();
#include "llvm/Support/TargetSelect.h"

#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetMachine.h"

#include "Test.h"
#include "aux/TestSymbolTable.h"
#include "aux/SymbolTable.h"

#include "aux/Environment.h"

#include "ast/Nil.h"


bool TestSymbolTable(std::ostream& out)
{
    /*
        Testing the various functions of the symbol table entails:

        -) SymbolTable's constructed against no outer scope have
            a nullptr for their OuterScope

        -) SymbolTable's Constructed 'inside' of another symbol table
            have a valid pointer to that symbol table

        -) Bind validly binds a symbol to a particular term because
            -) Lookup of an existing symbol returns that symbols binding
            
        #TODO
        -) Lookup finds symbols bound in the scope above, a property of
        	Lexical scoping.
        #TODO
        -) A local lookup will pnly search for names bound within the 
        	current scope, so we can tell the difference between 
        	free and bound names within scopes.

        -) Unbind successfully removes a particular binding because
            -) Lookup of a nonexisting symbol returns the empty optional

    */

    bool result = true;
    out << "\n-----------------------\n";
    out << "Testing pink::SymbolTable: \n";

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

    pink::SymbolTable t1(&bindings);

    result &= Test(out, "SymbolTable::OuterScope(), global scope", env.bindings.OuterScope() == nullptr);

    result &= Test(out, "SymbolTable::OuterScope(), inner scope", t1.OuterScope() == &bindings);

    pink::Location l(0, 5, 0, 7);
    llvm::Value* nil = env.ir_builder.getFalse();
    pink::Type*  nil_t = env.types.GetNilType();
    pink::InternedString x = env.symbols.Intern("x");

    env.bindings.Bind(x, nil_t, nil);
    llvm::Optional<std::pair<pink::Type*, llvm::Value*>> s0 = env.bindings.Lookup(x);

    result &= Test(out, "SymbolTable::Bind()", s0.hasValue());
    result &= Test(out, "SymbolTable::Lookup()", (*s0).first == nil_t && (*s0).second == nil);

    env.bindings.Unbind(x);
    llvm::Optional<std::pair<pink::Type*, llvm::Value*>> s1 = env.bindings.Lookup(x);

    result &= Test(out, "SymbolTable::Unbind()", !s1.hasValue());

    result &= Test(out, "pink::SymbolTable", result);
    out << "\n-----------------------\n";
    return result;
}
