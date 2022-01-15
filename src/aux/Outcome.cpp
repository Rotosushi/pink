

#include "aux/Outcome.h"

/*
namespace pink {
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
        : which(true), two(u)
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
            FatalError("Bad Outcome Access", __FILE__, __LINE__);
    }

    template <class T, class U>
    U Outcome<T, U>::GetTwo()
    {
        if (which)
            FatalError("Bad Outcome Access", __FILE__, __LINE__);
        else
            return two;
    }
}
*/
