#pragma once
#include <string>
#include <vector>

#include "llvm/IR/DataLayout.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"


#include "aux/Error.h"
#include "aux/StringInterner.h"
#include "aux/SymbolTable.h"
#include "aux/TypeInterner.h"

#include "ops/BinopTable.h"
#include "ops/UnopTable.h"

#include "front/Parser.h"



namespace pink {
    /*
        The Environment is the data structure that is passed
        through each phase of the compiler, where it is used
        to hold state for the future. So that the symbols,
        bindings, types etc.. created are saved for later use.
    */
    class Environment {
    public:
    	std::vector<InternedString> false_bindings;
        Parser&			   parser;
        StringInterner&    symbols;
        StringInterner&    operators;
        TypeInterner&      types;
        SymbolTable&       bindings;
        BinopTable&        binops;
        UnopTable&         unops;
        std::string&       target_triple;
        llvm::DataLayout&  data_layout;
        llvm::LLVMContext& context;
        llvm::Module&      module;
        llvm::IRBuilder<>& ir_builder;
        llvm::Function*    current_fn;

        Environment(Parser& p, StringInterner& si, StringInterner& oi, TypeInterner& ti,
                    SymbolTable& sy, BinopTable& bt, UnopTable& ut,
                    std::string& tt, llvm::DataLayout& dl,
                    llvm::LLVMContext& ct, llvm::Module& md,
                    llvm::IRBuilder<>& ib);
        
        // convience constructor for building an   
        // Environment around an inner scope. 
        Environment(Environment& env, SymbolTable& sy);
        
        // convience constructor for building an   
        // Environment around an inner scope. 
        Environment(Environment& env, SymbolTable& sy, llvm::IRBuilder<>& ib, llvm::Function* cf);
    };
}
