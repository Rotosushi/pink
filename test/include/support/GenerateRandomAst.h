#pragma once
#include <memory>


#include "ast/Ast.h"

#include "aux/Environment.h"
#include "aux/Outcome.h"

namespace pink {
    /*
        Broadly, there are two categories of Ast,
        Valid ones and Invalid ones.

        now, due to the way our Parser works we rely 
        on the fact that it does not generate an Ast 
        from source text which violates the grammar.

        so all Type and Semantic Errors get to Assume 
        that any Ast given does not violate any of an 
        entire class of possible Errors.

        When considering the possiblity of Generating a 
        random Ast, are we considering the entire set 
        of possible Ast's, or that subset which 
        is Parseable? or even more strongly the 
        subsets which are Parsable, Typecheck, Codegen,
        and whose emitted program behaves as expected?

        This is a super interesting question, and as stated 
        has no clear answer. all are valid choices.

        However we have an additional constraint.

        it would be neat to be able to fuzz test our 
        language. and one easy way of doing that would 
        be to emit random programs, which we happened to know
        would break in a specific way, or work in a specific 
        way.

        So, coming back to our original question, what sort of
        Abstract Syntax Tree do we want this function to return?

        I would say, a random ast which satisfied a particular property
        which is also known by, and returned from this procedure.

    */
    std::pair<std::unique_ptr<Ast>, Outcome<std::unique_ptr<Ast>, Error>> GenerateRandomAst(Environment& env, size_t* seed);
}