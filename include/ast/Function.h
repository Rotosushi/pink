/**
 * @file Function.h
 * @brief Header for class Function
 * @version 0.1
 */
#pragma once
#include <utility>
#include <vector>

#include "ast/Ast.h"
#include "aux/StringInterner.h"
#include "aux/SymbolTable.h"

#include "type/FunctionType.h"

#include "llvm/IR/DerivedTypes.h"

/*
        we need a way of calling any closure type without knowing the type of
   the closure. that is the way which we can handle calling a closure given
   accross a module boundary. that is, how can library code handle being given
   closures from user code. well, the function call site simply needs to be a
   static (unchanging) sequence of instructions that will work for any closure
   able to be bound to the function type given in the argument to the procedure.
   we are in a case where the library function has a parameter which is itself a
   function of some type, and user code is calling that library function passing
   in some closure to that parameter. That function could be one of many forms,
   due to circumstances that may arise when considering the syntactic forms of
   this ability to store functions as values.

        the first fact to observe is that a full function call is a point of
   similarity between the C procedure call, and the functional procedure call,
   (from the viewpoint of the lambda calculus) because it is only at this point
   when the actual body of the lambda will be evaluated. As all of the steps
   prior to this were binding names to values within the body. that is,
   assigning the contents of each of the parameters to this lamdba instance, and
   as such we can observe two distinct behaviors of a lambda, from the
   perspective of C. We can hold a lambda in a value position, and have no
   parameters bound or we can bind a parameter of that lambda. The outcome of
   binding the parameter is the same from the theoretical perspective, the
   lambda body evaluates to produce the inner lambda which itself needs a
   parameter to be bound before it's body evaluates. This process must be
        repeated until we reach the actual body which does work on the
   parameters themselves. this process is generally referred to as 'currying' in
   the literature. by way of currying the lambda calulus has multiple parameter
   procedures, which arise naturally given the definition of the calculus
   itself. despite the fact that per the rules of the calculus any function only
   has one parameter.

        so we can leverage this fact to separate out the functionality of lambda
   calulus into a form that is hopefully evaluatable on a mostly c like
   evaluation schema.

        so what I think we can leverage is a data-structure which represents the
   ability to bind parameters to a particular instance of itself. and then call
   the 'body' of the procedure when there are no more parameters left to bind.
        in this way, we can construct a new instance of a lambda by constructing
   a new instance of this data-structure. we can then bind parameters to values
   up until all parameters are bound, and then call this data-structure's
   function, passing in the very parameters which were bound.

        so we can see the problem that arises as a result of this now.
        The closure object's Type (!) changes (!) as a result of binding
        new parameters to the function. in this way, we can imagine a
        closure over some type of function, well as we move to the
        right of the function type, we can potentially match the function
        type of another function which truly has less parameters. and yet,
        despite the two potential closures having different underlying
        function types. After the parameters have become bound the closure
        could be bound to the parameter of another possible function
        and then that function body would have to call a closure which
        has more actual parameters than the type of the call site would
        seem to check for. luckily everything the call site misses, was
        itself already checked by another call site. but still, in this
        way a static call site taking a static typed function parameter is
        still asked to potentially apply a lambda which has many more
        arguments than the call site provides. in this circumstance we
        would have to rely on the parameters already bound within the
        lambda to fill in the rest of the parameters for the call site.
        luckily again those parameters are precisely those parameters which
        the underlying function needs, and if argument position is kept within
        the closure object, then we can even say that argument one lies at
        some position within the object. and then make a full call site of
        a particular instance of the closure.

        note that we cannot define a constant size for this data structure,
        because it's size is dependant upon the number of parameters to the
   function it represents and the sizes of those parameters.

        and there is one more subtle point to be made, even supposing we can
        construct a closure around a function, bind some parameters to it,
        and then appy that closure later giving it the full parameter list.
        we would still need to handle the case where instead of filling in
        all of the parameters the application site partially applies the
        given closure, and then stores it for later.  per the rules of the
        calculus this new value must be a new instance of the closure identical
        to the old instance yet containing the additional arguments which were
        bound during the partial application. and static library code must be
   able to perform partial application given any possible iteration of a
   closure. because again we must handle when a closure has already had it's
   type shrunk by the partial application process.

        so there are three distinct cases which need to be handled at every call
   site which could be passed a closure object. (luckily) we can observe which
   of these three cases is going to occur given the static type of that
   particular call site. Notice that when we fully apply the function, we are
   always providing the full number of parameters to the function type. so even
   if the closure itself we representing a larger function, that larger function
   has had all parameters bound up until the point where it's type matches the
   parameter type it is bound to, so a full application of the parameter would
   necessesarily mean a full application of any closure which was able to be
   bound to that parameter type.

        thus partial application is obvious in that it's result is always a
   function type again. (though this would still be a distinct case from a
   function which did work, and then returned a closure, as fully applying that
   function would result in another function being produced, except by the rules
   of the language, this would need to not be a closure around the called
   function, instead a storing of a return value)

        The one thing I can think of is leveraging an array of pointers to the
   memory allocated for the parameters. in this way we can translate the act of
   binding parameter (n) into the act of writing the memory pointed to by the
   nth pointer in the array. we can also translate the act of setting up the
   parameters for the function call by passing in all arguments as by-reference
   to these closed over values. in this way, setting up argument n for the
        function call means storing the nth pointer from the array into the
   corresponding argument register. (given that pointers always fit in registers
   we can pass any sized parameter this way)

        so by this schema I can see a static call site working based on an
   arbitrary array by also providing a few peices of information about said
   array, namely how large is the array. (how many parameters can be bound?) and
   how many parameters have already been bound. Then we can even see a
   translation for the action of partially binding some nth parameter, because
   we can retrieve the nth pointer, and perform a deep copy of the POD type with
   the destination being that nth pointer.

        the last thing to consider is the construction of a new instance of the
   closure we were given, without knowledge of that closures underlying TYPES!
   which seems nearly impossible! however, we can perhaps be clever with a
   memcpy? if we have a way of storing members within the closure itself which
   can tell inspecting code precisely how large the closure itself is. (in
   bytes), then we can copy the entire closure into a new allocation of that
   exact size by way of a memcpy. for this I was thinking about a void* to the
   last element of the entire closure object, that is a pointer to the last word
   of memory the closure itself occupies. then we can compute the size of the
   closure by performing pointer arithmetic from the pointer to the end of the
        object, minus the pointer to the beginning. and we already have the
   pointer to the beginning, as that is what a pointer to any object *is*. it is
   my understanding that such pointer arithmetic can be computed by static code.

        to recap:

        1) full application of a closure can be accomplished by moving the array
   of pointers to parameters into the correct argument registers of the function
   itself. then the function simply takes all of it's parameters by-reference.
   (but since we stored copies in the first place, it is semantically equivalent
   to by-value)

        2) partial application of a closure can be accomplished by:
                a) constructing a new allocation the same size as the given
   closure and memcpy'ing it's contents into this new allocation. this must
   include setting the newly invalidated dummy pointer to the end of the
   allocation to the correct location. also all of the newly invalidated
   parameter pointers. to accomplish this we could compute the offset of the
   parameter by giving it the same offset from the beginning which the original
   closure object had. say that for some given closure the byte difference
   between the pointer to the beginning of the object and the pointer to the nth
   argument was m, well then could we not compute the new nth argument pointer
   for the new closure by simply adding m to the pointer to the new allocation?
   this results in a new pointer into the new allocation which is in exactly the
   same relative position into the structure as the parameter had within the
   original closure. even if that actual member was offset from where you would
   expect given an array of allocations of the data-type due to alignment or
   padding, the relative position within the closure would simply copy whatever
                        alignment happened to be decided upon. simply by the
   fact that we already have a pointer to the nth element in the original
   closure.

                b) binding the nth parameter by writing the given value into the
   memory pointed to by the nth pointer in the array of parameter pointers.

        the boils down to the question of
        what if we put all the statically sized important bits at the front, and
   simply work with offsets from a void*? this avoids the issue of the fact that
   each particular closure must have a different POD type to store the
   parameters for a given function. by making every possible closure have all of
   the needed information at the same offset into the structure. such that any
   closure which could be processed, can be processed in the same way.

        addmitedly this is a lot of extra work, however one pleasant thing to
   note is that any direct full application to a function, (that is a legal
   function call from the perspective of C) can avoid all dynamic memory
   allocation, and can safely occur directly. that is, allowing for the
   difference that the caller must allocate space for all of the arguments being
   passed, and then clean it up after the call, to pass in parameters by
        reference to the given function. We can then jump directly to the
   function, with no need to construct a closure.

        anothe point to make is that of constructing a closure around a function
   definition, that is constructing a new closure with no arguments bound. This
   can be accomplished by constructing a new closure object, except that in this
   case we must construct a closure object for the given function type. that is
   this construction must allocate enough space for the entire closure and the
   point at which this happens knows the full function type that the closure
   represents. so there must be some POD involved in the sizeof computation.
   also something to note is that this can only occur when the programmer
   provides the full function definition to the call site. that is library code
   never needs to solve this problem, because the interface with the library is
   at the function call boundary. the library is not being expected to know the
   definitions of the programmers code. it simply accepts functions as
   parameters to procedures, which we require already that those are statically
   typed, and strictly evaluating.


        unique closure type per function where a
        union type of all argument types = ut

        closure type holds:
                i64            // the function pointer
                i64            // number of arguments
                i64            // number of bound arguments
                i64*           // pointer to the dummy at the end
                array of void* // points to each element within the array of ut
                array of ut    // buffer of all arguments in the closure
                i64            // dummy word at the end

        when we want to call a function, we simply pass each pointer from
        the array of void* in as each pointer typed argument to the function.
        then the function sees it's arguments as if they were by pointer,
        but since they are stored on the heap in the closure they are actually
        by value.
        Thus, a by value pointer typed argument would turn into a double pointer
   effectively.

        however this does seem rather sensible, because we can apply a pointer
   to a closure by getting the function pointer from the closure, then each of
   the void*'s into their argument positions, then simply calling the function
        as if it were a c style function with all pointer arguments.
        we always know the offset to the function pointer, the number of
   arguments, and the first element of the void* array from any given closure
   object so we can generate a sequence of instructions which can call any given
   function object (closure). just by computing offsets from it's pointer.
        similarly we can store the return value from the function in the closure
   buffer, just as if it were a sret argument, and thus store the return value
   for a given function call on the stack frame of the caller, and pass a
   pointer in during regular function application.

        to make this consistent we can simply transform our regular functions
   into c-style by pointer functions, and when we want to pass something by
   value we can make a copy on the stack before the function call.



*/

namespace pink {
/**
 * @brief Represents an instance of a Function expression
 *
 */
class Function : public Ast {
private:
  /**
   * @brief Compute the Type of this Function
   *
   * @param env the environment of this compilation unit
   * @return Outcome<Type*, Error> if true the Type of this Function,
   * if false the Error encountered.
   */
  [[nodiscard]] auto GetypeV(const Environment &env) const
      -> Outcome<Type *, Error> override;

  /**
   * @brief Emit the instructions which return a value from the main function
   *
   * @param env  the environment of this compilation unit
   * @param body_value the value of the body, to be returned.
   * @return Outcome<llvm::Value *, Error> if true, nullptr, if false the error
   * encountered.
   */
  static auto CodegenMainReturn(const Environment &env, llvm::Value *body_value)
      -> Outcome<llvm::Value *, Error>;

  /**
   * @brief Add the correct function parameter attributes given the defintion
   * of this function to the passed in llvm::Function.
   *
   * @param env the environment of this compilation unit
   * @param function the function to add parameter attributes too
   * @param function_type the llvm type of the function
   * @param p_function_type the pink type of the function
   * @return Outcome<llvm::Value *, Error> if true nullptr, if false the error
   * encountered
   */
  static auto
  CodegenParameterAttributes(const Environment &env, llvm::Function *function,
                             const llvm::FunctionType *function_type,
                             const pink::FunctionType *p_function_type)
      -> Outcome<llvm::Value *, Error>;

  /**
   * @brief Emit the instructions to create local variables for all arguments of
   * the given function
   *
   * @param env the environment of this compilation unit
   * @param function the function to emit arguments for
   * @param p_function_type the pink function type of the function
   */
  void CodegenArgumentInit(const Environment &env,
                           const llvm::Function *function,
                           const pink::FunctionType *p_function_type) const;

public:
  /**
   * @brief The name of this Function
   *
   */
  InternedString name;

  /**
   * @brief The formal arguments of this Function
   *
   */
  std::vector<std::pair<InternedString, Type *>> arguments;

  /**
   * @brief The Body expression of this Function
   *
   */
  std::unique_ptr<Ast> body;

  /**
   * @brief The local scope of this Function
   *
   */
  std::shared_ptr<SymbolTable> bindings;

  /**
   * @brief Construct a new Function
   *
   * @param location the textual location of this Function
   * @param name the name of this Function
   * @param arguments the formal arguments to this Function
   * @param body the Body expression of this Function
   * @param outer_scope the Scope this Function resides within
   */
  Function(const Location &location, const InternedString name,
           std::vector<std::pair<InternedString, Type *>> arguments,
           std::unique_ptr<Ast> body, SymbolTable *outer_scope);

  /**
   * @brief Destroy the Function
   *
   */
  ~Function() override = default;

  Function(const Function &other) = delete;

  Function(Function &&other) = default;

  auto operator=(const Function &other) -> Function & = delete;

  auto operator=(Function &&other) -> Function & = default;

  /**
   * @brief Implements LLVM style [RTTI] for this class
   *
   * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
   *
   * @param ast the ast to test
   * @return true if ast *is* an instance of Function
   * @return false if ast *is not* an instance of Function
   */
  static auto classof(const Ast *ast) -> bool;

  /**
   * @brief Compute the cannonical string representation of this Function
   *
   * @return std::string the string representation
   */
  [[nodiscard]] auto ToString() const -> std::string override;

  /**
   * @brief Compute the value of this Function
   *
   * @param env the environment of this compilation unit
   * @return Outcome<llvm::Value*, Error> if true the llvm::Function
   * representing this Function, if false the Error encountered.
   */
  [[nodiscard]] auto Codegen(const Environment &env) const
      -> Outcome<llvm::Value *, Error> override;
};
} // namespace pink
