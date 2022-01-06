#pragma once
#include <string>
#include <variant>

#include "llvm/IR/DataLayout.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"


#include "aux/Error.hpp"
#include "aux/StringInterner.hpp"
#include "aux/SymbolTable.hpp"
#include "aux/TypeInterner.hpp"
//#include "aux/PrecedenceTable.hpp"
//#include "aux/BinopTable.hpp"
//#include "aux/UnopTable.hpp"
//#include "front/Parser.hpp"

//#TODO Finish This after all the files above


namespace pink {
    /*
        The Environment is the data structure that is passed
        through each phase of the compiler, where it is used
        to hold state for the future. So that the symbols,
        bindings, types etc.. created are saved for later use.
    */
    class Environment {
    public:
        // Parser parser;
        StringInterner& symbols;
        StringInterner& operators;
        TypeInterner&   types;
        SymbolTable&    bindings;
        // PrecedenceTable op_prec;
        // BinopTable  binops;
        // UnopTable   unops;
        std::string&       target_triple;
        llvm::DataLayout&  data_layout;
        llvm::LLVMContext& context;
        llvm::Module&      module;
        llvm::IRBuilder<>& ir_builder;

        Environment(StringInterner& si, StringInterner& oi, TypeInterner& ti,
                    SymbolTable& sy, std::string& tt, llvm::DataLayout& dl,
                    llvm::LLVMContext& ct, llvm::Module& md,
                    llvm::IRBuilder<>& ib);
    };
}
