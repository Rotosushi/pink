/**
 * @file Outcome.h
 * @brief Header for class Outcome
 * @version 0.1
 * 
 */
#pragma once
#include <utility> //
#include "aux/Error.h"

namespace pink {

   /**
    * @brief A container holding one of two alternatives, but not both.
    * 
    * \warning this class cannot be typed (legally constructed) if both alternative
    * types are the same type. This is fine however, because if you want to 
    * return the same type in two different states to represent the return 
    * value of a given function, that can be accomplished by simply using that
    * type directly as the return value of that function.
    * 
    * @tparam T the first alternative's type
    * @tparam U the second alternative's type
    */
    template <class T, class U>
    class Outcome {
    private:
        /**
         * @brief which alternative is currently active
         * true when the first alternative is active
         * false when the second alternative is active
         */
        bool which;

        /**
         * @brief the first alternative
         * 
         */
        T one;

        /**
         * @brief the second alternative
         * 
         */
        U two;

    public:
        /**
         * @brief there is no way to construct an Outcome without providing one of the alternatives
         * 
         */
        Outcome() = delete;

        /**
         * @brief Destroy the Outcome
         * 
         */
        ~Outcome();

        /**
         * @brief Construct a new Outcome, holding a first alternative equal to t
         * 
         * @param t the value of the first alternative
         */
        Outcome(const T& t);

        /**
         * @brief Construct a new Outcome, holding a first alternative equal to t
         * 
         * @param t the value of the first alternative
         */
        Outcome(T&& t);

        /**
         * @brief Construct a new Outcome, holding a second alternative equal to u
         * 
         * @param u the value of the second alternative
         */
        Outcome(const U& u);

        /**
         * @brief Construct a new Outcome, holding a second alternative equal to u
         * 
         * @param u the value of the second alternative
         */
        Outcome(U&& u);

        /**
         * @brief Construct a new Outcome, equal to another outcome
         * 
         * @param other the other Outcome to copy the contents of
         */
        Outcome(const Outcome& other);
        Outcome(Outcome&& other);

        /**
         * @brief Assign this Outcome to the value of the given first alternative t
         * 
         * @param t the value of the first alternative
         * @return Outcome& this Outcome
         */
        Outcome& operator=(const T& t);

        /**
         * @brief Assign this Outcome to the value of the given second alternative u
         * 
         * @param u the value of the second alternative
         * @return Outcome& this Outcome
         */
        Outcome& operator=(const U& u);

        /**
         * @brief Assign this Outcome to the value of another Outcome
         * 
         * @param other the other Outcome to copy
         * @return Outcome& this Outcome
         */
        Outcome& operator=(const Outcome& other);
        Outcome& operator=(Outcome&& other);
        

        /**
         * @brief returns which alternative is held
         * 
         * @return true when the first alternative is held
         * @return false when the second alternative is held
         */
        operator bool();

        /**
         * @brief Get which alternative is held
         * 
         * @return true when the first alternative is held
         * @return false when the second alternative is held
         */
        bool GetWhich();

        /**
         * @brief Get the first alternative
         * 
         * @return T& the member t
         */
        T& GetFirst();

        /**
         * @brief Get the second alternative
         * 
         * @return U& the member u
         */
        U& GetSecond();
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
    T& Outcome<T, U>::GetFirst()
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
    U& Outcome<T, U>::GetSecond()
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
