

#include "aux/Environment.hpp"



namespace pink {
    Environment::Environment(StringInterner& si, StringInterner& oi, TypeInterner& ti,
                SymbolTable& sy, std::string& tt, llvm::DataLayout& dl,
                llvm::LLVMContext& ct, llvm::Module& md,
                llvm::IRBuilder<>& ib)
        : symbols(si), operators(oi), types(ti), bindings(sy),
          target_triple(tt), data_layout(dl), context(ct), module(md),
          ir_builder(ib)
    {

    }
}
