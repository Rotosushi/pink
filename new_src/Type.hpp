#pragma once
#include <string>
#include <memory>

/* TODO: implement a simple type heirarchy for
  an internal representation of

  type := Nil
        | Int
        | Bool
        | type -> type

  then, codegen of type literals computes the llvm::Type
  and we can use TypeLiterals same as before.

  typechecking however can utilize the 'internal' types.
  thus, we can distinguish between Nil and Bool, despite
  their physical representations being identical.
*/
