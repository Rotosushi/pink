#include "kernel/BinopPrimitives.h"
#include "ops/BinopTable.h"


namespace pink {
    
  
    Outcome<llvm::Value*, Error> BinopIntAdd(llvm::Type* lty, llvm::Value* left, llvm::Type* rty, llvm::Value* right, std::shared_ptr<Environment> env)
    {
      if (lty != rty)
        FatalError("Binop int add type mismatch", __FILE__, __LINE__);
      
      llvm::Value* value = env->builder->CreateAdd(left, right, "iadd");
      Outcome<llvm::Value*, Error> result(value);
      return result;
    }

    Outcome<llvm::Value*, Error> BinopIntSub(llvm::Type* lty, llvm::Value* left, llvm::Type* rty, llvm::Value* right, std::shared_ptr<Environment> env)
    {
      if (lty != rty)
        FatalError("Binop int sub type mismatch", __FILE__, __LINE__);
    	
      llvm::Value* value = env->builder->CreateSub(left, right, "isub");
      Outcome<llvm::Value*, Error> result(value);
      return result;
    }

    Outcome<llvm::Value*, Error> BinopIntMul(llvm::Type* lty, llvm::Value* left, llvm::Type* rty, llvm::Value* right, std::shared_ptr<Environment> env)
    {
      if (lty != rty)
        FatalError("Binop int mult type mismatch", __FILE__, __LINE__);
    	
      llvm::Value* value = env->builder->CreateMul(left, right, "imul");
      Outcome<llvm::Value*, Error> result(value);
      return result;
    }

    Outcome<llvm::Value*, Error> BinopIntSDiv(llvm::Type* lty, llvm::Value* left, llvm::Type* rty, llvm::Value* right, std::shared_ptr<Environment> env)
    {
      if (lty != rty)
        FatalError("Binop int sdiv type mismatch", __FILE__, __LINE__);
    	
      llvm::Value* value = env->builder->CreateSDiv(left, right, "idiv");
      Outcome<llvm::Value*, Error> result(value);
      return result;
    }

    Outcome<llvm::Value*, Error> BinopIntMod(llvm::Type* lty, llvm::Value* left, llvm::Type* rty, llvm::Value* right, std::shared_ptr<Environment> env)
    {
    	if (lty != rty)
        FatalError("Binop int mod type mismatch", __FILE__, __LINE__);
      
      llvm::Value* value = env->builder->CreateSRem(left, right, "imod");
      Outcome<llvm::Value*, Error> result(value);
      return result;
    }

    Outcome<llvm::Value*, Error> BinopIntEq(llvm::Type* lty, llvm::Value* left, llvm::Type* rty, llvm::Value* right, std::shared_ptr<Environment> env)
    {
    	if (lty != rty)
        FatalError("Binop int eq type mismatch", __FILE__, __LINE__);
      
      llvm::Value* value = env->builder->CreateICmpEQ(left, right, "ieq");
      Outcome<llvm::Value*, Error> result(value);
      return result;
    }

    Outcome<llvm::Value*, Error> BinopBoolEq(llvm::Type* lty, llvm::Value* left, llvm::Type* rty, llvm::Value* right, std::shared_ptr<Environment> env)
    {
      if (lty != rty)
        FatalError("Binop bool eq type mismatch", __FILE__, __LINE__);
    	
      llvm::Value* value = env->builder->CreateICmpEQ(left, right, "booleq");
      Outcome<llvm::Value*, Error> result(value);
      return result;
    }

    Outcome<llvm::Value*, Error> BinopIntNe(llvm::Type* lty, llvm::Value* left, llvm::Type* rty, llvm::Value* right, std::shared_ptr<Environment> env)
    {
      if (lty != rty)
        FatalError("Binop int neq type mismatch", __FILE__, __LINE__);
      
      llvm::Value* value = env->builder->CreateICmpNE(left, right, "ineq");
      return Outcome<llvm::Value*, Error> (value);
    }

    Outcome<llvm::Value*, Error> BinopBoolNe(llvm::Type* lty, llvm::Value* left, llvm::Type* rty, llvm::Value* right, std::shared_ptr<Environment> env)
    {
      if (lty != rty)
        FatalError("Binop bool neq type mismatch", __FILE__, __LINE__);
      
      llvm::Value* value = env->builder->CreateICmpNE(left, right, "boolneq");
      return Outcome<llvm::Value*, Error> (value);
    }

    Outcome<llvm::Value*, Error> BinopIntGt(llvm::Type* lty, llvm::Value* left, llvm::Type* rty, llvm::Value* right, std::shared_ptr<Environment> env)
    {
      if (lty != rty)
        FatalError("Binop int gt type mismatch", __FILE__, __LINE__);
      
      llvm::Value* value = env->builder->CreateICmpSGT(left, right, "igt");
      return Outcome<llvm::Value*, Error> (value);
    }
    
    Outcome<llvm::Value*, Error> BinopIntGe(llvm::Type* lty, llvm::Value* left, llvm::Type* rty, llvm::Value* right, std::shared_ptr<Environment> env)
    {
      if (lty != rty)
        FatalError("Binop int ge type mismatch", __FILE__, __LINE__); 
      
      llvm::Value* value = env->builder->CreateICmpSGE(left, right, "ige");
      return Outcome<llvm::Value*, Error> (value);
    }
    
    Outcome<llvm::Value*, Error> BinopIntLt(llvm::Type* lty, llvm::Value* left, llvm::Type* rty, llvm::Value* right, std::shared_ptr<Environment> env)
    {
     if (lty != rty)
        FatalError("Binop int lt type mismatch", __FILE__, __LINE__);
      
      llvm::Value* value = env->builder->CreateICmpSLT(left, right, "ilt");
      return Outcome<llvm::Value*, Error> (value);
    }
   
    Outcome<llvm::Value*, Error> BinopIntLe(llvm::Type* lty, llvm::Value* left, llvm::Type* rty, llvm::Value* right, std::shared_ptr<Environment> env)
    {
      if (lty != rty)
        FatalError("Binop int le type mismatch", __FILE__, __LINE__);    
      
      llvm::Value* value = env->builder->CreateICmpSLE(left, right, "ile");
      return Outcome<llvm::Value*, Error> (value);
    }

    Outcome<llvm::Value*, Error> BinopBoolAnd(llvm::Type* lty, llvm::Value* left, llvm::Type* rty, llvm::Value* right, std::shared_ptr<Environment> env)
    {
      if (lty != rty)
        FatalError("Binop bool and type mismatch", __FILE__, __LINE__);
   	  
      llvm::Value* value = env->builder->CreateAnd(left, right, "and");
      Outcome<llvm::Value*, Error> result(value);
      return result;
    }

    Outcome<llvm::Value*, Error> BinopBoolOr(llvm::Type* lty, llvm::Value* left, llvm::Type* rty, llvm::Value* right, std::shared_ptr<Environment> env)
    {
      if (lty != rty)
        FatalError("Binop bool or type mismatch", __FILE__, __LINE__);
    	
      llvm::Value* value = env->builder->CreateOr(left, right, "or");
      Outcome<llvm::Value*, Error> result(value);
      return result;
    }

    Outcome<llvm::Value*, Error> BinopPtrLeftAdd(llvm::Type* lty, llvm::Value* left, llvm::Type* rty, llvm::Value* right, std::shared_ptr<Environment> env)
    {
    /*
     * compute the address of the i'th element in the array pointed to
     * by left. where right holds the value of i. We consider an array 
     * to decompose into a simple pointer to it's zero'th offset. 
     * this means that given a pointer to a simple type, we only need to 
     * compute a GEP 1, and not a GEP 2. because the offset relative to 
     * the pointer will be the pointee type offset from the original location.
     * this differs from how we would index a pointer to an array, because when 
     * we compute the first offset away from a pointer to an array type, we 
     * would be computing the offset of the first array after the end of the 
     * given array.
     */
      //llvm::PointerType* lpty = llvm::cast<llvm::PointerType>(left->getType());
      //llvm::Type* ety = lpty->getPointerElementType();
      if (lty == nullptr)
        FatalError("lty was nullptr!", __FILE__, __LINE__);
      
      if (rty == nullptr)
        FatalError("rty was nullptr!", __FILE__, __LINE__);

      std::vector<llvm::Value*> indicies = { right };
      llvm::Value* gep = env->builder->CreateGEP(lty, left, indicies, "gep" /* isInBounds = true*/);
      return Outcome<llvm::Value*, Error>(gep);
    }

    Outcome<llvm::Value*, Error> BinopPtrRightAdd(llvm::Type* lty, llvm::Value* left, llvm::Type* rty, llvm::Value* right, std::shared_ptr<Environment> env)
    {
      /*
       *  This is the same behavior as BinopPtrLeftAdd, except that we assume 
       *  the integer is the left term and the pointer is the right term.
       *
       */
      if (lty == nullptr)
        FatalError("lty was nullptr!", __FILE__, __LINE__);
      
      if (rty == nullptr)
        FatalError("rty was nullptr!", __FILE__, __LINE__);
      
      std::vector<llvm::Value*> indicies = { left, };
      llvm::Value* gep = env->builder->CreateGEP(rty, right, indicies, "gep" /* isInBounds = true*/);
      return Outcome<llvm::Value*, Error>(gep);
    }

    void InitializeBinopPrimitives(std::shared_ptr<Environment> env)
    {
        /*
            precedence table:
                ==        : 1
                < <= > >= : 2
                & |       : 3
                + -       : 4
                * / %     : 5
        */
        InternedString minus = env->operators->Intern("-");
        InternedString plus  = env->operators->Intern("+");
        InternedString mul   = env->operators->Intern("*");
        InternedString div   = env->operators->Intern("/");
        InternedString mod   = env->operators->Intern("%");
        InternedString land  = env->operators->Intern("&");
        InternedString lor   = env->operators->Intern("|");
        InternedString cmpeq = env->operators->Intern("==");
        InternedString cmpne = env->operators->Intern("!=");
        InternedString cmplt = env->operators->Intern("<");
        InternedString cmple = env->operators->Intern("<=");
        InternedString cmpgt = env->operators->Intern(">");
        InternedString cmpge = env->operators->Intern(">=");

        Type* int_ty  = env->types->GetIntType();
        Type* bool_ty = env->types->GetBoolType();
        Type* int_ptr_ty  = env->types->GetPointerType(int_ty);
        Type* bool_ptr_ty = env->types->GetPointerType(bool_ty);

        Associativity left_assoc = Associativity::Left;

        env->binops->Register(cmpeq, 1, left_assoc, int_ty, int_ty, bool_ty, BinopIntEq);
        env->binops->Register(cmpeq, 1, left_assoc, bool_ty, bool_ty, bool_ty, BinopBoolEq);
        env->binops->Register(cmpne, 2, left_assoc, int_ty, int_ty, bool_ty, BinopIntNe);
        env->binops->Register(cmpne, 2, left_assoc, bool_ty, bool_ty, bool_ty, BinopBoolNe);
        env->binops->Register(cmplt, 2, left_assoc, int_ty, int_ty, bool_ty, BinopIntLt);
        env->binops->Register(cmple, 2, left_assoc, int_ty, int_ty, bool_ty, BinopIntLe);
        env->binops->Register(cmpgt, 2, left_assoc, int_ty, int_ty, bool_ty, BinopIntGt);
        env->binops->Register(cmpge, 2, left_assoc, int_ty, int_ty, bool_ty, BinopIntGe);
        env->binops->Register(land,  3, left_assoc, bool_ty, bool_ty, bool_ty, BinopBoolAnd);
        env->binops->Register(lor,   3, left_assoc, bool_ty, bool_ty, bool_ty, BinopBoolOr);
        env->binops->Register(plus,  4, left_assoc, int_ty, int_ty, int_ty, BinopIntAdd);
        env->binops->Register(minus, 4, left_assoc, int_ty, int_ty, int_ty, BinopIntSub);
        env->binops->Register(mul,   5, left_assoc, int_ty, int_ty, int_ty, BinopIntMul);
        env->binops->Register(div,   5, left_assoc, int_ty, int_ty, int_ty, BinopIntSDiv);
        env->binops->Register(mod,   5, left_assoc, int_ty, int_ty, int_ty, BinopIntMod);
       
        // well, either these need to be removed in favor of a full on Ast node
        // which handles array indirection. or we need to define an overload
        // for every potential array length, or we need to change the type that 
        // an array is bound to within the program to a pointer, so this
        // solution works... 
        env->binops->Register(plus,  4, left_assoc, int_ptr_ty, int_ty, int_ptr_ty, BinopPtrLeftAdd);
        env->binops->Register(plus,  4, left_assoc, int_ty, int_ptr_ty, int_ptr_ty, BinopPtrRightAdd);
        env->binops->Register(plus,  4, left_assoc, bool_ptr_ty, int_ty, bool_ptr_ty, BinopPtrLeftAdd);
        env->binops->Register(plus,  4, left_assoc, int_ty, bool_ptr_ty, bool_ptr_ty, BinopPtrRightAdd);
    }
}
