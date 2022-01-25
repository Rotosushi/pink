#pragma once
#include <utility> // std::pair<>

#include "llvm/ADT/APInt.h" // llvm::Optional<>
#include "llvm/ADT/DenseMap.h" // llvm::DenseMap<...>
#include "llvm/IR/Value.h" // llvm::Value

#include "aux/StringInterner.h" // pink::InternedString
#include "type/Type.h" // pink::Type


namespace pink {
/*
	The SymbolTable is a tricky data-structure here.
	we want to be able to lookup bindings during both 
	typechecking and code generation. However we have two 
	distinct values that need to be bound during either.
	
	Now, normally this is not an issue at all, because 
	llvm::Value has a method getType(), which returns 
	the type of the value. And for our intents and purposes 
	up until the point where we add functions to the langauge 
	llvm::Type*'s work fantastically. However because we are 
	mapping a more abstract version of functions onto 
	llvm's more low-level hardware perspective of functions 
	we need to manage the types of the functions ourselves 
	for Typechecking. Since this problem is central to the 
	conception of the language itself we would be remiss to 
	simply avoid the issue here and leave solving the problem 
	for later when we add functions.
	
	Now, we are already planning to, and have a consistent mapping 
	from our primitive types to llvm's Types. However if we 
	choose to have the symboltable hold llvm::Values, then it would 
	necessecitate a reverse mapping when we want to report the 
	type of a bound symbol to the typechecker during that phase 
	of compilation.
	With how a partially bound procedure is actually typed within 
	the language, that becomes tricky itself as any type 
	shortening that happens as a result of partial application
	would not necessarily be represented in the 
	type of the function from the perspective of llvm.
	which means that such a reverse mapping would not easily
	be consistent with the langauges types, which is a problem.
	
	If we instead have the symboltable hold pink::Ast*'s 
	then we are in a weird situation when we go to implement 
	the codegeneration portion of the language,
	as it is far easier to simply hold the SSA values themselves 
	for direct use in further terms of the language.
	for instance, binary operators take two llvm::Values
	to generate the binary operation in llvm. if the 
	symboltable stores symbols, such as local variables and 
	arguments directly, then the code to generate an operation 
	upon two of these SSA values becomes very straightforward.
	
	And one of the problems of requireing that we generate 
	new llvm::Values each time we perform a lookup is that 
	how does llvm then connect those values together.
	like, as it is currently, the parameter to a procedure 
	is a SSA value, if we bind it internally as an Ast, and generate 
	a new SSA value for each useage of said value, that means 
	a whole bunch of unique SSA values are being used at each 
	site, and not the same value being used multiple times.
	this seems to break a lot of the assumptions that llvm 
	uses to perform program optimizations and such, if such 
	a codegenerator would work at all.
	
	There might be a workaround though, if we define a new 
	Ast node which simply holds an llvm::Value, then we 
	can bind llvm::Values directly into the symboltable, even though 
	it only holds Ast nodes. Then the 'codegeneration' of such a 
	node would be to simply return the llvm::Value held within 
	the node itself. Then we can implement something like 
	binding a new local variable (an llvm::AllocaInst*) 
	by binding an Ast node holding that very same llvm::Value.
	then each usage of said binding would be able to unwrap
	the returned binding into the correct SSA value for each 
	use of the name. This then also covers the usecase during 
	typechecking, if we also define a Ast node which wraps 
	types within the language. This particular usecase is 
	when we need to check the type of a bound value within 
	the body of a procedure. Although, this might need more 
	thought regarding the JIT and how that evaluation model
	is going to work. Because we are then talking about 
	potentially needing to use the type of a preevaluated 
	symbol during the evaluation of the next statement.
	where the only record of the type of the previous symbol 
	is the particular SSA value that was generated during 
	it's evaluation. If we are talking about the type of the 
	symbol of something like a lambda value. the actual 
	function generated remember is distinct in it's type 
	from the type of it within the conception of the language.
	
	having slept on this, we might also modify the 
	SymbolTable to map InternedStrings to a 
	std::pair<llvm::Value*, pink::Type*>
	which leaves this as one single table, and 
	allows both Getype and Codegen to use the same table 
	to do their work. then the typechecker can 
	create temporary bindings during typing of a function body,
	as the typechecker only cares about the types of the bindings. 
	if codegen needs the pink::Type it has it, and 
	if it needs the llvm::Type is has acess to it through 
	llvm::Value::getType().
	
	The one constraint that this poses is that in order to 
	construct a binding, we must know the type. Which should 
	be able to be met in all cases, as we have to type the 
	binding statement before we evaluate it down to a value.
	Which should mean that anytime we are actually creating 
	a binding, we have already typed the expression being 
	bound to the symbol.
*/
    class SymbolTable {
    private:
        llvm::DenseMap<InternedString, std::pair<Type*, llvm::Value*>> map;
        SymbolTable* outer;

    public:
        SymbolTable();
        SymbolTable(const SymbolTable& other) = delete;
        SymbolTable(SymbolTable* outer);
        ~SymbolTable();

        SymbolTable* OuterScope();

        llvm::Optional<std::pair<Type*, llvm::Value*>> Lookup(InternedString symbol);

        void Bind(InternedString symbol, Type* type, llvm::Value* term);
        void Unbind(InternedString symbol);
    };
}
