

#include "aux/Environment.h"



namespace pink {
    Environment::Environment(Parser& p, StringInterner& si, StringInterner& oi, TypeInterner& ti,
                SymbolTable& sy, BinopTable& bt, UnopTable& ut,
                std::string& tt, llvm::DataLayout& dl,
                llvm::LLVMContext& ct, llvm::Module& md,
                llvm::IRBuilder<>& ib)
        : parser(p), symbols(si), operators(oi), types(ti), bindings(sy), binops(bt),
          unops(ut), target_triple(tt), data_layout(dl), context(ct),
          module(md), ir_builder(ib), current_fn(nullptr)
    {

    }
    
    Environment::Environment(Environment& env, SymbolTable& sy)
    	: parser(env.parser), symbols(env.symbols), operators(env.operators),
    	  types(env.types), bindings(sy), binops(env.binops), unops(env.unops),
    	  target_triple(env.target_triple), data_layout(env.data_layout),
    	  context(env.context), module(env.module), ir_builder(env.ir_builder),
    	  current_fn(env.current_fn)
    {
    
    }
    
    Environment::Environment(Environment& env, SymbolTable& sy, llvm::IRBuilder<>& ib, llvm::Function* cf)
    	: parser(env.parser), symbols(env.symbols), operators(env.operators),
    	  types(env.types), bindings(sy), binops(env.binops), unops(env.unops),
    	  target_triple(env.target_triple), data_layout(env.data_layout),
    	  context(env.context), module(env.module), ir_builder(ib),
    	  current_fn(cf)
    {
    
    }
}
