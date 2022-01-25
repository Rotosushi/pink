

#include "aux/Environment.h"



namespace pink {
    Environment::Environment(Parser& p, StringInterner& si, StringInterner& oi, TypeInterner& ti,
                SymbolTable& sy, BinopTable& bt, UnopTable& ut,
                std::string& tt, llvm::DataLayout& dl,
                llvm::LLVMContext& ct, llvm::Module& md,
                llvm::IRBuilder<>& ib)
        : parser(p), symbols(si), operators(oi), types(ti), bindings(sy), binops(bt),
          unops(ut), target_triple(tt), data_layout(dl), context(ct),
          module(md), ir_builder(ib)
    {

    }
}
