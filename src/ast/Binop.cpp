#include "ast/Binop.h"
#include "aux/Environment.h"

namespace pink {
    Binop::Binop(Location& loc, InternedString o, std::unique_ptr<Ast> l, std::unique_ptr<Ast> r)
        : Ast(Ast::Kind::Binop, loc), op(o), left(std::move(l)), right(std::move(r))
    {

    }

    Binop::~Binop()
    {

    }

    std::unique_ptr<Ast> Binop::Clone()
    {
        return std::make_unique<Binop>(loc, op, left->Clone(), right->Clone());
    }

    bool Binop::classof(const Ast* t)
    {
        return t->getKind() == Ast::Kind::Binop;
    }

    std::string Binop::ToString()
    {
    	std::string result;
    	if (llvm::isa<Binop>(left))
    		result += "(" + left->ToString() + ")";
    	else 
    		result += left->ToString();
    		
    	result += " " + std::string(op) + " ";
    	
    	if (llvm::isa<Binop>(right))
    		result += "(" + right->ToString() + ")";
    	else 
    		result += right->ToString();
    		
        return result;
    }
    
    /*
    	env |- lhs : Tl, rhs : Tr, op : Tl -> Tr -> T
      --------------------------------------------------
      			  env |- lhs op rhs : T
    */
    Outcome<Type*, Error> Binop::GetypeV(std::shared_ptr<Environment> env)
    {
    	// Get the type of both sides
    	Outcome<Type*, Error> lhs_result(left->Getype(env));
    	
    	if (!lhs_result)
    		return lhs_result;
    		
    	Outcome<Type*, Error> rhs_result(right->Getype(env));
    	
    	if (!rhs_result)
    		return rhs_result;
    		
    	// find the operator present between both sides in the env 
    	llvm::Optional<std::pair<InternedString, BinopLiteral*>> binop = env->binops->Lookup(op);
    	
    	if (!binop)
    	{
        std::string errmsg = std::string("unknown op: ")
                           + op;
    		Error error(Error::Code::UnknownBinop, loc, errmsg);
    		Outcome<Type*, Error> result(error);
    		return result;
    	}

      // #TODO: unop also needs this treatment for * and & operators.
      llvm::Optional<std::pair<std::pair<Type*, Type*>, BinopCodegen*>> literal;
      PointerType* pt = nullptr;
      if (((pt = llvm::dyn_cast<PointerType>(lhs_result.GetOne())) != nullptr) && (strcmp(op, "+") == 0))
      {
        literal = binop->second->Lookup(lhs_result.GetOne(), rhs_result.GetOne());

        IntType* it = llvm::dyn_cast<IntType>(rhs_result.GetOne());
        if (it == nullptr)
        {
          std::string errmsg = std::string("lhs has pointer type: ")
                             + pt->ToString()
                             + " however rhs is not an Int, rhs has type: "
                             + rhs_result.GetOne()->ToString();
          Error error(Error::Code::ArgTypeMismatch, loc, errmsg);
          return Outcome<Type*, Error>(error);
        }

        if (!literal)
        {
          Type* int_ptr_type = env->types->GetPointerType(env->types->GetIntType());
          llvm::Optional<std::pair<std::pair<Type*, Type*>, BinopCodegen*>> ptr_add_binop = binop->second->Lookup(int_ptr_type, it);
          
          if (!ptr_add_binop)
            FatalError("Couldn't find int ptr arithmetic binop!", __FILE__, __LINE__);
          
          BinopCodegenFn ptr_arith_fn =  ptr_add_binop->second->generate;
          literal = binop->second->Register(lhs_result.GetOne(), it, lhs_result.GetOne(), ptr_arith_fn); 
        }
      }
      else
      {  
    	  // find the instance of the operator given the type of both sides
    	  literal = binop->second->Lookup(lhs_result.GetOne(), rhs_result.GetOne());
      }

    	if (!literal)
    	{
        std::string errmsg = std::string("could not find an implementation of ")
                           + std::string(op)
                           + " for given types, left: "
                           + lhs_result.GetOne()->ToString()
                           + ", right: "
                           + rhs_result.GetOne()->ToString();
        Error error(Error::Code::ArgTypeMismatch, loc, errmsg);
    		Outcome<Type*, Error> result(error);
    		return result;
    	}
    	
    	// return the result type of calling the operator given the types of both sides
    	Outcome<Type*, Error> result(literal->second->result_type);
    	return result;
    }
    
    
    Outcome<llvm::Value*, Error> Binop::Codegen(std::shared_ptr<Environment> env)
    {
      // Get the type and value of both sides
    	Outcome<Type*, Error> lhs_type_result(left->Getype(env));
    	
    	if (!lhs_type_result)
    		return Outcome<llvm::Value*, Error>(lhs_type_result.GetTwo());


    	Outcome<llvm::Type*, Error> lhs_type_codegen = lhs_type_result.GetOne()->Codegen(env);
    	
    	if (!lhs_type_codegen)
    		return Outcome<llvm::Value*, Error>(lhs_type_codegen.GetTwo());

      llvm::Type* lhs_type = lhs_type_codegen.GetOne();
  
      

    	Outcome<llvm::Value*, Error> lhs_value(left->Codegen(env));
    	
    	if (!lhs_value)
    		return lhs_value;

    	Outcome<Type*, Error> rhs_type_result(right->Getype(env));
    	
    	if (!rhs_type_result)
    		return Outcome<llvm::Value*, Error>(rhs_type_result.GetTwo());
    

    	Outcome<llvm::Type*, Error> rhs_type_codegen = rhs_type_result.GetOne()->Codegen(env);
    		
    	if (!rhs_type_codegen)
    		return Outcome<llvm::Value*, Error>(rhs_type_codegen.GetTwo());
  
      llvm::Type* rhs_type = rhs_type_codegen.GetOne();

    	Outcome<llvm::Value*, Error> rhs_value(right->Codegen(env));
    	
    	if (!rhs_value)
    		return rhs_value;
  

    	// find the operator present between both sides in the env 
    	llvm::Optional<std::pair<InternedString, BinopLiteral*>> binop = env->binops->Lookup(op);
    	
    	if (!binop)
    	{
        std::string errmsg = std::string("unknown op: ")
                           + op;
    		Error error(Error::Code::UnknownBinop, loc, errmsg);
    		Outcome<llvm::Value*, Error> result(error);
    		return result;
    	}
    	
      llvm::Optional<std::pair<std::pair<Type*, Type*>, BinopCodegen*>> literal;
      PointerType* pt = nullptr;
      // if this is a pointer addition operation, we must pass the pointee_type
      // into the binop code generation function for the GEP instruction within.
      // we can also safely add a new implementation of pointer addition no
      // matter what the underlying type is, because the GEP instruction
      // handles the offset computation based upon the pointee_type itself.
      // it is only the structure of the BinopLiteral itself that is going to 
      // fail to find an implementation for any arbitrary array type we can 
      // define.
      if (((pt = llvm::dyn_cast<PointerType>(lhs_type_result.GetOne())) != nullptr) && (strcmp(op, "+") == 0))
      {
        Outcome<llvm::Type*, Error> pointee_type_result = pt->pointee_type->Codegen(env);
       
        if (!pointee_type_result)
          return Outcome<llvm::Value*, Error>(pointee_type_result.GetTwo());

        lhs_type = pointee_type_result.GetOne();

        IntType* it = llvm::dyn_cast<IntType>(rhs_type_result.GetOne());
        if (it == nullptr)
        {
          std::string errmsg = std::string("lhs has pointer type: ")
                             + pt->ToString()
                             + " however rhs is not an Int, rhs has type: "
                             + rhs_type_result.GetOne()->ToString();

          Error error(Error::Code::ArgTypeMismatch, loc, errmsg);
          return Outcome<llvm::Value*, Error>(error);
        }

        literal = binop->second->Lookup(lhs_type_result.GetOne(), it);
  
        if (!literal)
        {
          Type* int_ptr_type = env->types->GetPointerType(env->types->GetIntType());
          llvm::Optional<std::pair<std::pair<Type*, Type*>, BinopCodegen*>> ptr_add_binop = binop->second->Lookup(int_ptr_type, it);
          
          if (!ptr_add_binop)
            FatalError("Couldn't find int ptr arithmetic binop!", __FILE__, __LINE__);
          
          BinopCodegenFn ptr_arith_fn =  ptr_add_binop->second->generate;
          literal = binop->second->Register(lhs_type_result.GetOne(), it, lhs_type_result.GetOne(), ptr_arith_fn); 
        }
      }
      else
      {
        // find the instance of the operator given the type of both sides
    	  literal = binop->second->Lookup(lhs_type_result.GetOne(), rhs_type_result.GetOne());
      }

    	if (!literal)
    	{
        std::string errmsg = std::string("could not find an implementation of ")
                           + std::string(op)
                           + " for the given types, left: "
                           + lhs_type_result.GetOne()->ToString()
                           + ", right: "
                           + rhs_type_result.GetOne()->ToString();
    		Error error(Error::Code::ArgTypeMismatch, loc, errmsg);
    		Outcome<llvm::Value*, Error> result(error);
    		return result;
    	}
    	
    	// use the lhs and rhs values to generate the binop expression.
    	Outcome<llvm::Value*, Error> binop_value(literal->second->generate(lhs_type, lhs_value.GetOne(), rhs_type, rhs_value.GetOne(), env));
    	return binop_value;
    }
}
