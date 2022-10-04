#include <random>

#include "support/GenerateRandomAst.h"

#include "ast/Application.h"
#include "ast/Array.h"
#include "ast/Assignment.h"
#include "ast/Bind.h"
#include "ast/Binop.h"
#include "ast/Block.h"
#include "ast/Bool.h"
#include "ast/Conditional.h"
#include "ast/Dot.h"
#include "ast/Function.h"
#include "ast/Int.h"
#include "ast/Nil.h"

#include "type/ArrayType.h"
#include "type/BoolType.h"
#include "type/FunctionType.h"
#include "type/IntType.h"
#include "type/NilType.h"
#include "type/PointerType.h"
#include "type/TupleType.h"


namespace pink {
    /*
        We are going to define a set of mutually recursive procedures which 
        generate the Asts themselves and the Errors or expected success values.

        (as an added bonus if we write the function to generate the Ast based upon
        a seed value, then we can get the same random tree from the generator given 
        the same seed. this makes it possible to recreate the "random" tree to reproduce 
        potentially buggy code. what this means is simply seeding srand with the seed 
        and then calling rand, which is already pseudo-random itself. then when 
        we call rand within the generator it will be the same sequence of random 
        numbers based upon the seed.)

        Generally speaking it will be one procedure per Ast node kind,
        and that procedure will use some random number to generate some 
        portion of it's sub trees. Then also based on that random number 
        it will only make a descision to create a new subtree if it can know
        what that subtree will result in. That is the behavior of that subtree.
        So it can also record that behavior. Then once that is accomplished 
        for each subtree, we will have a randomly generated Ast along with it's
        expected behavior. 

        To keep things extra simple we will only generate a subset of the languages
        full capabilities to begin with, and add more functionality as we learn more.

        To generate a random main program we simply define a no-arg main that returns 
        either a boolean or integer value. 

        the question begged is what does the value hold?

        to keep this simple we generate a single term which has result type 
        Bool or Int depending upon the return Type of main.

        when generating a term we have a few terms which can result in a value,
        variables, unary operations, and binary operations.

        to keep this simple we are going to stick to only operations on the Types 
        Bool and Int, saving Ptr, Array, and Struct for later.
        additionally we are going to save while loops and conditionals for later, because the way they
        'return' values is by modifying variables, so we need to be able to handle 
        using the result of a variable defined in one term being used in the second 
        term. once we have that in place its simply a matter of ensuring the while 
        loop does work on a variable known to the while loops defining scope so that 
        can be used with the next statement generated, ditto for conditionals.

        we can generate variable definitions by generating random symbols of a 
        given type with a random constant. we simply need to keep track of 
        them in some sort of symbol table.

        we can then generate unary operations by selecting a random unary operator 
        appropriate to the required Type, and then randomly selecting either a variable of 
        the appropriate Type, or a constant of the appropriate type to be the rhs.
        or a function application of the appropriate Type.

        then at any point where the selection must be appropriate we can deterministically 
        inject an Error by deliberately selecting the wrong Type of subterm

        we can generate binary operations in much the same way as we generate unops,
        the appropriate points are:
            selecting the operator
            selecting the rhs
            selecting the lhs

        we can generate conditionals in much the same way, 
        the appropriate points are:
            selecting the test expression
            selecting the first alternative expression
            selecting the second alternative expression

    */

    

    std::unique_ptr<Ast> GenerateRandomBasic(Environment& env, std::mt19937_64& gen, pink::Type* requested_type)
    {
        std::unique_ptr<Ast> result_ast;

        std::uniform_int_distribution<> zero_to_ten(0, 10);
        std::uniform_int_distribution<> zero_to_(0, 5);

        // we could just as well construct a variable term with type Int, 
        // or a Unop Term with type Int.
        // But how can Arrays and Tuples fit into this schema?
        // they cannot appear in as many leaf locations as a 
        // pure integer, and the way they are handled must be specialized 
        // compared to regular integers.
        if (pink::IntType* it = llvm::dyn_cast<pink::IntType>(requested_type))
        {
            result_ast = std::make_unique<pink::Int>(Location(), zero_to_ten(gen));
        }

        return result_ast;
    }

    std::pair<std::unique_ptr<Ast>, Outcome<std::unique_ptr<Ast>, Error>> GenerateRandomAst(Environment& env, size_t* seed)
    {
        // a real random number generator, (if available)
        std::random_device rng;

        // a pseudo random number generator based on the 'mersenne twister'
        std::mt19937_64 gen;

        // either use the seed provided, or 
        // generate a random seed.
        // it might be useful to return the 
        // seed used from this procedure.
        if (seed != nullptr)
            gen.seed(*seed);
        else
            gen.seed(rng());
    }
}