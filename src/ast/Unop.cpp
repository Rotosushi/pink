#include "ast/Unop.h"
#include "aux/Environment.h"

namespace pink {
    Unop::Unop(Location& loc, InternedString o, std::unique_ptr<Ast> r)
        : Ast(Ast::Kind::Unop, loc), op(o), right(std::move(r))
    {

    }

    Unop::~Unop()
    {

    }

    std::unique_ptr<Ast> Unop::Clone()
    {
        return std::make_unique<Unop>(loc, op, right->Clone());
    }

    bool Unop::classof(const Ast* t)
    {
        return t->getKind() == Ast::Kind::Unop;
    }

    std::string Unop::ToString()
    {
        return std::string(op) + right->ToString();
    }
    
    /*
    	    env |- rhs : Tr, op : Tr -> T
      ----------------------------------------
      			env |- op rhs : T
    */
    Outcome<Type*, Error> Unop::GetypeV(std::shared_ptr<Environment> env)
    {
    	// get the type of the rhs
    	Outcome<Type*, Error> rhs_result(right->Getype(env));
    	
    	if (!rhs_result)
    		return rhs_result;
    	
    	// find the operator used in the env
    	llvm::Optional<std::pair<InternedString, UnopLiteral*>> unop = env->unops->Lookup(op);
    	
    	if (!unop)
    	{
    		Error error(Error::Code::UnknownUnop,	loc);
    		Outcome<Type*, Error> result(error);
    		return result;
    	}
    	
    	// find the overload of the operator for the given type
    	llvm::Optional<std::pair<Type*, UnopCodegen*>> literal;
      PointerType* pt = nullptr;
      if (((pt = llvm::dyn_cast<PointerType>(rhs_result.GetOne())) != nullptr) && (strcmp(op, "*") == 0))
      {
        literal = unop->second->Lookup(rhs_result.GetOne());
        
        if (!literal)
        {
          Type* int_ptr_type = env->types->GetPointerType(env->types->GetIntType());
          llvm::Optional<std::pair<Type*, UnopCodegen*>> ptr_indirection_unop = unop->second->Lookup(int_ptr_type);

          if (!ptr_indirection_unop)
          {
            FatalError("Couldn't find int ptr indirection unop!", __FILE__, __LINE__);
          }

          UnopCodegenFn ptr_indirection_fn = ptr_indirection_unop->second->generate;
          literal = unop->second->Register(pt, pt->pointee_type, ptr_indirection_fn);
        } 
      }
      else if (strcmp(op, "&") == 0)
      {
        literal = unop->second->Lookup(rhs_result.GetOne());
        
        if (!literal)
        {
          PointerType* pt = env->types->GetPointerType(rhs_result.GetOne());
          Type* int_type = env->types->GetIntType();
          llvm::Optional<std::pair<Type*, UnopCodegen*>> address_of_unop = unop->second->Lookup(int_type);

          if (!address_of_unop)
          {
            FatalError("Couldn't find int address_of unop!", __FILE__, __LINE__);
          }

          UnopCodegenFn address_of_fn = address_of_unop->second->generate;
          literal = unop->second->Register(rhs_result.GetOne(), pt, address_of_fn);
        }
      }
      else
      {
        literal = unop->second->Lookup(rhs_result.GetOne());
      }

    	if (!literal)
    	{
    		Error error(Error::Code::ArgTypeMismatch, loc);
    		Outcome<Type*, Error> result(error);
    		return result;
    	}
    	
    	// return the result type of applying the operator to the given type
    	Outcome<Type*, Error> result(literal->second->result_type);
    	return result;
    }
    
    
    /*

    	    env |- rhs : Tr, op : Tr -> T
      ----------------------------------------
      			env |- op rhs : unop.generate(rhs.value)

    */
    Outcome<llvm::Value*, Error> Unop::Codegen(std::shared_ptr<Environment> env)
    {
    	// get the type of the right hand side for operator lookup
    	Outcome<Type*, Error> rhs_type(right->Getype(env));
    	
    	if (!rhs_type)
      {
    		return Outcome<llvm::Value*, Error>(rhs_type.GetTwo());
      }

    	// get the value to generate the llvm ir

      Outcome<llvm::Value*, Error> rhs_value(Error(Error::Code::None, Location()));
      
      if (strcmp(op, "&") == 0)
      {
        // we cannot assign to an address value, we can only assign to 
        // a memory location.
        if (env->flags->OnTheLHSOfAssignment())
        {
          return Outcome<llvm::Value*, Error>(Error(Error::Code::ValueCannotBeAssigned, loc));
        }

        // take the address of the rhs,
        //
        // all setting this flag does is stop Variable::Codegen
        // from emitting a load instruction on the pointer to 
        // memory the Variable is bound to. 
        // I am fairly sure this is the only thing we can take 
        // address of, but this is a place where we might see unexpected
        // behavior as a result of this descision.
        env->flags->WithinAddressOf(true);

        rhs_value = right->Codegen(env);

        env->flags->WithinAddressOf(false);
      }
      else if (strcmp(op, "*") == 0)
      {
        // dereference the rhs
        // if we are on the lhs of an assignment expression 
        // only emit one load, else we want to load the 
        // value from the pointer that we retrieved from
        // loading the pointer the variable was bound to.
        if (env->flags->OnTheLHSOfAssignment())
        {
          // emit one load. which Variable::Codegen already does for us,
          // so we just have to pretend we aren't on the lhs of assignment
          env->flags->OnTheLHSOfAssignment(false);
          env->flags->WithinDereferencePtr(true);

          rhs_value = right->Codegen(env);

          env->flags->OnTheLHSOfAssignment(true);
          env->flags->WithinDereferencePtr(false);
        }
        else 
        {
          // emit two loads, so we just have to emit a load on the llvm::Value*
          // Codegening the rhs returned, so tell Variable::Codegen that we 
          // want to dereference the pointer it has
          env->flags->WithinDereferencePtr(true);

          rhs_value = right->Codegen(env);

          env->flags->WithinDereferencePtr(false);

          if (!rhs_value)
          {
            return rhs_value;
          }

          // strip off the ptr type so we can construct a load of the value
          // within the ptr.
          // we know that the Type of the rhs is a PointerType, otherwise the 
          // typechecker would have never typed this expression.
          PointerType* ptr_type = llvm::cast<pink::PointerType>(rhs_type.GetOne());

          Outcome<llvm::Type*, Error> llvm_pointee_type = ptr_type->pointee_type->Codegen(env);

          if (!llvm_pointee_type)
          {
            return Outcome<llvm::Value*, Error>(llvm_pointee_type.GetTwo());
          }

          if (llvm_pointee_type.GetOne()->isSingleValueType())
          {
            rhs_value = env->builder->CreateLoad(llvm_pointee_type.GetOne(), rhs_value.GetOne(), "deref");    
          }
          // else emit no load for types that cannot be loaded
        }
      }
      else // just emit the rhs as we normally do for unops
      {
        rhs_value = right->Codegen(env);

        if (!rhs_value)
        {
          return rhs_value;
        }
      }

    	// find the literal
    	llvm::Optional<std::pair<InternedString, UnopLiteral*>> unop = env->unops->Lookup(op);
    	
    	if (!unop)
    	{
        Error error(Error::Code::UnknownUnop,	loc);
    		Outcome<llvm::Value*, Error> result(error);
    		return result;
    	}
    	
    	// find the overload of the operator for the given type
    	llvm::Optional<std::pair<Type*, UnopCodegen*>> literal = unop->second->Lookup(rhs_type.GetOne());
    	
    	if (!literal)
    	{
    		Error error(Error::Code::ArgTypeMismatch, loc);
    		Outcome<llvm::Value*, Error> result(error);
    		return result;
    	}
    	
    	// use the operators associated generator expression to generate the llvm ir 
    	Outcome<llvm::Value*, Error> unop_value(literal->second->generate(rhs_value.GetOne(), env));
    	// either outcome, return the result.
    	return unop_value;
    }
}
