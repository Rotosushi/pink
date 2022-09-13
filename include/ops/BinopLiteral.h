#pragma once
#include <utility> // std::pair
#include <memory>  // std::unique_ptr

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/DenseMap.h"

#include "ops/BinopCodegen.h"
#include "ops/PrecedenceAndAssociativity.h"

namespace pink {

    class BinopLiteral {
    private:
        // because pink::Types are interned, std::pair<>::operator ==
        // works how we would want it too, namely, pointers can be used 
        // as unique handles to each type programmatically.
        llvm::DenseMap<std::pair<Type*, Type*>, std::unique_ptr<BinopCodegen>> overloads;

    public:
        Precedence precedence;
        Associativity associativity;
        bool isDefault;

        BinopLiteral() = delete;
        // #TODO: consider this:
        // on one hand, if we are creating a binop literal without an
        // associated implementation, it is almost certainly a default 
        // binop and thus it seems perfectly reasonable to make the default 
        // value of isDefault true for convenience. additionally, if you 
        // have an associated implementation it is most likely that the binop 
        // is being defined by the user, and thus will not be a default
        // binop literal. on the other hand, it might be confusing to users of 
        // the api of this class that one constructor's default value for
        // isDefault is different than the other constructor.
        BinopLiteral(Precedence p, Associativity a, bool isDefault = true);
        BinopLiteral(Precedence p, Associativity a, Type* left_t, Type* right_t, Type* ret_t, BinopCodegenFn fn, bool isDefault = false);
        ~BinopLiteral();

        unsigned NumOverloads() const;

        std::pair<std::pair<Type*, Type*>, BinopCodegen*> Register(Type* left_t, Type* right_t, Type* ret_t, BinopCodegenFn fn);

        void Unregister(Type*  left_t, Type* right_t);

        llvm::Optional<std::pair<std::pair<Type*, Type*>, BinopCodegen*>> Lookup(Type* left_t, Type* right_t);
    };
}
