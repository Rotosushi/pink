#pragma once
#include <utility> //
#include "aux/Error.h"

namespace pink {
    /*
        A simple class which allows one to pass one of two
        alternatives through a single datatype.

        used during parsing, typechecking, and codegeneration
        to handle  the possiblity of syntax, type, or semantic
        errors.

        There is a slight bug in this class, where if you instanciate
        with two of the same class then the constructors become ambiguous,
        so the build doesn't compile, but it works for the use-case,
        and it's a comptime error not a runtime one, so that makes it
        much more bearable.

    */
    template <class T, class U>
    class Outcome {
    private:
        bool which;
        T one;
        U two;

    public:
        Outcome() = delete;
        ~Outcome();
        Outcome(const T& t);
        Outcome(T&& t);
        Outcome(const U& u);
        Outcome(U&& u);
        Outcome(const Outcome& other);
        Outcome(Outcome&& other);

        Outcome& operator=(const T& t);
        Outcome& operator=(const U& u);
        Outcome& operator=(const Outcome& other);
        Outcome& operator=(Outcome&& other);
        operator bool();
        bool GetWhich();

        T& GetOne();
        U& GetTwo();
    };

    template <class T, class U>
    Outcome<T, U>::~Outcome()
    {

    }
    
    template <class T, class U>
    Outcome<T, U>::Outcome(const T& t)
        : which(true), one(t), two()
    {

    }


    template <class T, class U>
    Outcome<T, U>::Outcome(T&& t)
        : which(true), one(std::forward<T>(t)), two()
    {

    }
    
    template <class T, class U>
    Outcome<T, U>::Outcome(const U& u)
        : which(false), one(), two(u)
    {

    }

    template <class T, class U>
    Outcome<T, U>::Outcome(U&& u)
        : which(false), one(), two(std::forward<U>(u))
    {

    }


    template <class T, class U>
    Outcome<T, U>::Outcome(const Outcome& other)
        : which(other.which)
    {
        if (which)
            one = other.one;
        else
            two = other.two;
    }
    
    template <class T, class U>
    Outcome<T, U>::Outcome(Outcome&& other)
    	: which(other.which)
    {
    	if (which)
    		one = other.one;
    	else 
    		two = other.two;
    }

    template <class T, class U>
    Outcome<T, U>& Outcome<T, U>::operator=(const T& t)
    {
      which = true;

      one = t;

      return *this;
    }

    template <class T, class U>
    Outcome<T, U>& Outcome<T, U>::operator=(const U& u)
    {
      which = false;

      two = u;

      return *this;
    }
    
    template <class T, class U>
    Outcome<T, U>& Outcome<T, U>::operator=(const Outcome& other)
    {
        which = other.which;

        if (which)
            one = other.one;
        else
            two = other.two;
           
        return *this;
    }
    
    template <class T, class U>
    Outcome<T, U>& Outcome<T, U>::operator=(Outcome&& other)
    {
        which = other.which;

        if (which)
            one = std::move(other.one);
        else
            two = std::move(other.two);
           
        return *this;
    }

    template <class T, class U>
    Outcome<T, U>::operator bool()
    {
        return which;
    }

    template <class T, class U>
    bool Outcome<T, U>::GetWhich()
    {
        return which;
    }

    template <class T, class U>
    T& Outcome<T, U>::GetOne()
    {
        if (which)
            return one;
        else
        {
            FatalError("Bad Outcome Access", __FILE__, __LINE__);
            return one; // suppress warning
        }

    }

    template <class T, class U>
    U& Outcome<T, U>::GetTwo()
    {
        if (which)
        {
            FatalError("Bad Outcome Access", __FILE__, __LINE__);
            return two; // suppress warning
        }
        else
            return two;
    }
}
