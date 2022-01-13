#pragma once

#include "aux/Error.hpp"

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
        union {
            T one;
            U two;
        };

    public:
        Outcome() = delete;
        ~Outcome();
        Outcome(T t);
        Outcome(U u);
        Outcome(const Outcome& other);

        operator bool();
        bool GetWhich();

        T GetOne();
        U GetTwo();
    };

    template <class T, class U>
    Outcome<T, U>::~Outcome()
    {

    }

    template <class T, class U>
    Outcome<T, U>::Outcome(T t)
        : which(true), one(t)
    {

    }

    template <class T, class U>
    Outcome<T, U>::Outcome(U u)
        : which(false), two(u)
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
    T Outcome<T, U>::GetOne()
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
    U Outcome<T, U>::GetTwo()
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
