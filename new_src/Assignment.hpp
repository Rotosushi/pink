#pragma once
#include <string>
#include <memory>

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Ast.hpp"

/*
  can we implement assignment as a binop,
  or are we forced to define a new Ast?
  is there a material
  difference between assignment to a name,
  (names are secret pointers)
  x <- ...; y <- ...;
  assignment to a pointer, (i.e. if the expression
  computes an address which can be assigned to.)
  x.member <- ...; x[index] <- ...;
  z : Ref Int <- &x; *z <- ...;
  and the incorrect term which attempts to
  assign to a literal value.
  32 <- 0;
  and can we see that difference with the
  given definition of binary operators?

  in the case of the binop, we are given two fully
  evaluated terms, which in the case of a
  name appearing next to the operator, means
  replacing that name with it's value, which
  in reality means loading it's value into
  working memory (registers).

  even though, assignment has the same grammatical form
  as binary operators.
  and hey wait, names -are- pointers. and expressions that
  compute pointers are usually there as intermediate
  results to further read or write the data that
  we are pointing to. don't we just need a value
  and a pointer to that value?
  (can we type assignment as Ref T '<-' T)
  that means we need the same amount of information
  about the sides as within the binop expression.


  well, an argument i can see is that within the
  binop we use a concrete type selection method
  to disambiguate between the overload set of a
  binary operator. with assignment we now need to
  define an assignment function per type. how can
  we extend this method when we want to implement
  user defined types? i mean, we are already implementing
  user defined binops, couldn't we use the same mechanism,
  just make it a function with the right type signature?
  i.e. we assume the operator (<-) is overloaded for every type.
  and when we want to extend the overload set to the new type
  by defining a new procedure taking a Ref T on the left
  and a T on the right, where T is the user type.
  we can generate a new procedure by emitting a sequence of
  instructions amounting to a BuildStore as the body of
  said procedure. if we define this procedure as a
  Lambda (or maybe a formal procedure), and register
  that lambda as the procedure to call
  when we encounter a Ref T and a T surrounding the '<-' symbol
  just like we do when a user defines a lambda (or formal procedure)
  of their own symbol/or overloading an existing symbol, for their own types.

  it's so close, the only problem is that this is assuming that
  names are references, and that we have access to a llvm::PointerType
  literal. but this is unfortunately not the case, formally we
  Load the value from it's alloca when we encounter the name
  in the grammar, to get the current value when the name is
  used in a larger expression. this is precisely counter to
  what we want when we are implementing assignment, we need
  the address of the location to store the value, not the
  value stored at that location. so, because names are secret
  references, and not explict references, we have to implement
  assignment as an Ast node. however, we have learned a lot
  about how to properly implement Assignment.
  it's essentially a load followed by a store.

  okay okay, what if we didn't return the loaded value when
  we 'evaluate' the name. what if we simply return a pointer
  via treating the llvm::AllocaInst as a llvm::Value.
  (upcasts are always safe remember.)
  now, this means that within code that is consuming evaluated
  tree's no longer gets to assume that we are receiving a
  constant value every time. this complicates code everywhere else
  that wants to process constant values, like operators, and
  arguments. (which is, like, everything?)
  if we allow outside code to load these values, we will perform
  the final step of evaluation before we can compute the current
  step of evaluation. and if we consider a variable that is storing
  an address, in which case we would recieve a ptr to a ptr to type.
  this single emitted load, as opposed to strictly unwrapping all
  encountered ptrs would still allow a ptr value to be manipulated by
  the text of the program.
  additionally the unwraped type can be known prior to unwrapping, so
  we can emit an error against the expected unwraped type not matching
  the formally defined type for this operation.
  the benefit of this approach is now assignment will formally
  receive a:
       Ref T and a Ref T,          ~> which is assigning a pointed to value to a pointer
  or a Ref T and a T               ~> which is assigning a constant to a pointer
  or a T and either a Ref T or a T ~> which is assigning anything to a constant.

  so we have our three cases;
  ~> assigning a referenced value into a memory location
  ~> assigning a directly computed constant into a memory location
  ~> erroneously assigning something to a constant or computed constant.

  now,
  x.member <- ...; x[index] <- ...;
  z : Ref Int <- &x; *z <- ...;

  the dot (id '.' id) operator can compute a Ref T to the struct member
  and assignment can use the pointer into a subsection of the alloca space
  for valid assignment.

  the array subscript/bracket operator (id '[' affix : Index ']')
  can compute a Ref T to the array element and then assignment can
  use that for valid assignment.

  variable references/identifier compute the address of their
  value, instead of the value directly, so that assignment can
  use this Ref to perform assignment.

  the & operator can compute the address of any identifier,
  this is where this approach becomes weird.

  z :<- x // means a by-value copy of the contents of x within z.
  // Ref T <- Ref T

  and we are designing this around the fact that we want this semantic
  around assignment by default.

  z :<- &x // means a by-value copy of the address of x into the location z
  // Ref T <- Ref Ref T

  this means that the & operator needs to take the Ref T it gets from
  an identifier, store it into a local allocation, and return the address
  of that. then Assignment, (and any other place that want to use the
  one free dereference rule, to load variables) looks at the wrapped
  type, and see's that it is a Ref T itself,  the programmer
  typed something amounting to

  &x + 9 // which is not integer arithmetic persay.

  would be typed as Ref T '+' T which is not defined
  for the operator '+'. in fact the type comparison
  mechanism would need to be expanded to handle any
  given type, including user types. we could attach
  a generator procedure that defined a new overload
  when we try and take the address of a type we
  havent before. because the operation is always
  the same, we take a Ref T we store it in a new
  local alloca and then return the Ref T of that
  new alloca. which returns the address as the
  value we find ourselves unwrapping with the
  one-free-dereference rule. we are always passing
  a typed pointer around from the perspective of
  llvm, but we can hold it as a simple llvm::Value*
  so the same implementation procedure can be called
  by the binop codegen procedure, but to the mapping
  we created, we are talking about distinct implementations
  for every given type.


  pointer arithmetic should be handled
  by casting the number or the pointer
  or defining a form of the operator that
  can take a Ref T, though, this touches on
  the issues with assignment, where I don't
  quite remember if our equality comparison
  for types against llvm::PointerTypes would
  compare equal regardless of wrapped type.
  this means we need a function body per type.
  which is overkill. we may need to implement it
  as an new Ast kind unary operator. like how
  Assignment is a new Ast kind binary operator.)

  so, any literal that is too big to fit in memory
  directly should be handled by storing it into a
  local allocation? then operators taking those types
  can be defined taking references to those types?



*/

namespace {

class Assignment : public Ast
{
private:
  std::shared_ptr<Ast> left;
  std::shared_ptr<Ast> right;

  virtual Judgement GetypeV(const Environment& env) override;
public:
  Assignment(const Location& loc, std::shared_ptr<Ast> left, std::shared_ptr<Ast> right);
  virtual ~Assignment() = default;

  virtual std::shared_ptr<Assignment> Clone() override;
  virtual std::string ToString() override;

  static bool classof(Ast* ast);

  virtual Judgement Codegen(const Environment& env) override;
};

}
