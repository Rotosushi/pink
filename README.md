# pink

Pink is a simple idea from a simple problem.

I have written a lot of c code. 
I love a lot of things about the langauge
but there are a few sticking points:
	*) inclusion via textual macros is clever,
		way too clever. so instead I want 
		a more modern treatment of namespaces.
		with out-of-order declarations supported 
		by the language.
	*) c functions are nice and very simple,
		but they could be better, what if 
		we allowed name overloading and 
		parametric polymorphism? with 
		the compiler inferring a dynamic 
		dispatch method per overload-set 
		and parametric polymorphic set.
		dynamic-dispatch via v-tables is 
		accepted within the 'fast' language 
		of c++, so having primitive function 
		calls equivalent to c function calls,
		with the ability to have a more modern
		style of coding, seems awesome.
		(which includes static type applications
		of overloaded procedures, as
		the dispatch can be resolved at compile time)
	*) a more modern conceptualization of types,
		with constructors and such. so 
		we can fold away type construction 
		such that we can provide a suite of 
		construction procedures instead of 
		having to report the physical structure of 
		the type to everyone. this isn't always 
		the thing to do, especially if the type 
		needs inspected by multiple disjoint parts of 
		the program, but it can allow us to program
		'to the interface' rather than the implementation,
		which helps us to decouple the parts of our 
		program, which helps in larger projects.
	*) a lack of a string primitive alongside the char primitive.
		cmon, why do i have to #include <string> 
		every time?
		it's so basic, what useful program that 
		interacts with the user can elide text 
		entirely?
