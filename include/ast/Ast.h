#pragma once
#include <memory>
#include <string>
#include <iostream>

#include "llvm/Support/Casting.h"

#include "aux/Location.h"
#include "aux/Error.h"
#include "aux/Outcome.h"

#include "type/Type.h"

#include "llvm/IR/Value.h"

namespace pink {
    class Environment;

    class Ast {
    public:
      // https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html
      enum class Kind {
        Block,
        Variable,
        VarRef,
        Bind,
        Assignment,
        Dot,
        Function,
        Application,
        Conditional,
        While,

        Tuple,
        Array,

        Binop,
        Unop,

        Nil,
        Bool,
        Int,
      };

    protected:
      const Kind kind;
      Location   loc;
      Type*      type;
        
    private:
    	virtual Outcome<Type*, Error> GetypeV(const Environment& env) = 0;

    public:
      Ast(const Kind k, Location l);
      virtual ~Ast();

      Kind getKind() const;
      Location GetLoc() const;
 
      // we have a few extra arguments compared to the basic visitor pattern
      // but this is to pass data too and from the visitor so it can actually 
      // work. if we pass in a AstVisitor& to the accept function, then we 
      // are forced to simply trust the programmer to provide the correct 
      // visitor for the Outcome<T, U>& result they pass in, and the 
      // result must be an argument because we cannot overload a function 
      // on it's return type, and we cannot define a single accept method for 
      // any given Visitor implementation because we cannot template a virtual method! 
      // the only way to handle this is to reimplement the Outcome class
      // to use inheritance to handle the difference between a codegen outcome or 
      // a typecheck outcome or so on. however, then we are trusting that the 
      // outcome that we receive as an argument is the correct derived Outcome,
      // which means that is still not as type safe as the current way of implementing
      // the 'visitors'.
      // as far as i can tell right now, this is about how we have to implement
      // the visitor pattern from the perspective of the Ast.
      // because of:
      // 1) we need some way of communicating the resulting type of a typecheck,
      //    or resulting value of a codegen, or resulting string of a ToString,
      //    and it needs to work such that the Typechecking/codegeneration/TOString
      //    routines can use the results from calling the same routine on the 
      //    child nodes of the current ast node. this is the basic way the algorithm
      //    builds up the type of expressions.
      // 2) we need to do the work with respect to the Environment data structure,
      //    because that is where the SymbolTable, BinopTable, UnopTable, llvm::IRBuilder,
      //    and other important data structures are grouped, such that codegeneration 
      //    and typechecking can be done with respect to those data structures. 
      //    this is fundamentally because we must record symbols as they are defined
      //    such that we can access their values later to implement expressions
      //    referencing those symbols.
      //void Accept(const TypeChecker &tc, Outcome<Type*, Error>& result, std::shared_ptr<Environment> env) = 0;
      //void Accept(const CodeGenerator &cg, Outcome<llvm::Value*, Error>& result, std::shared_ptr<Environment> env) = 0;
      //void Accept(const StringGenerator &sg, std::string& result, std::shared_ptr<Environment> env) = 0;
      
      // the visitor pattern increases code size and mental complexity, because
      // we add a new class definition to the program per kind of walk we want 
      // to do. I guess if we had numerous kinds of walks then each Ast node's
      // file would have to contain snippets of several algorithms in the same source 
      // file, so we gain the ease of declaring the entire algorithm in a single 
      // source file, implementing the visitor for each ast node kind. so in that 
      // sense the algorithm is easier to see all at once. however each class kind
      // takes similar steps within it's peice of each algorithm. so that becomes 
      // harder to see if the whole algorithm is within the visitor file.
      // also, the whole typechecking algorithm is about as big as the parser,
      // so the size of the typechecking file would still be big, and get bigger 
      // as we added new node kinds.
      // but the Ast code as it stands now gets bigger each time we add a new kind 
      // of walk to the tree. 
      // it seems to me that debugging and profiling code would be emitted alonside 
      // the code that is already emitted, and they don't need to be their own
      // walks of the tree. just have flags set that control the emission.
      // so what kind of walks do we need to add to the AST now or in the future?
      // ah, each optimization that we would want to add would need to walk the 
      // tree, and it would be easier to build and maintain separate classes for 
      // each optimization kind, rather than increase the code size of the Ast,
      // per optimization supported.
      // this might be a consideration if the code we emit to llvm cannot be
      // optimized by llvm well, due to the way we are emitting the llvm 
      // representation. so we could employ optimization walks to 
      // modify such code. so adding the visitor pattern can be safely put off
      // until we have a need to add another kind of walk.
            

      virtual std::unique_ptr<Ast> Clone() = 0;

      /*
      #TODO
        This function ends up doing a lot of 
        intermediate allocations and concatenations,
        perhaps we could rewrite this to use 
        llvm::Twine or something similar?
        
      #TODO
        This function should be aware of the level of 
        nesting, and properly indent the code it emits.
      */
      virtual std::string ToString() = 0;

      /*
        #TODO: Getype can be made to cache 
          the computed type in the Ast 
          class itself, which should save
          on computation within Codegen.
      */
      Outcome<Type*, Error> Getype(const Environment& env);
      virtual Outcome<llvm::Value*, Error> Codegen(const Environment& env) = 0;
    };
}
