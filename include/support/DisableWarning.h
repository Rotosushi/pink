/**
 * @file DisableWarning.h
 * @brief Header defining the macro NOWARN
 * @version 0.1
 */
#pragma once
// This is to be used only when there is no other
// sensible option for dealing with a specific
// warning,
// we have -Wall -Wextra turned on during
// development to catch unsafe code, and so 
// code is developed with a goal of no warnings 
// first.
// NOTE: 9/15/2022
// These are only defined for clang and GCC
// because those are the two compilers I have been 
// using to build this project. However this 
// macro may certainly be defined for each compiler
// that supports this style of dignostic push/pop
// and specific error ingnoring.
// NOTE: 9/15/2022
// Outline of this code retrieved from 
// https://stackoverflow.com/questions/48426484/concise-way-to-disable-specific-warning-instances-in-clang

/**
 * @brief helper macro for NOWARN
 * 
 */
#define  DO_PRAGMA(x) _Pragma(#x)

/*
NOTE 9/15/2022
Turns out that this version of the macro is redundant as clang supports 
both clang diagnostic push
and  GCC diagnostic push
so when compiling with clang we get the 
-Wmacro-redefined warning because clang defines
both __clang__ and __GNUC__,

and on that note, there is no __XXXX___ which is defined if
and only if GCC is the compiler currently being used.
see:
-> https://gcc.gnu.org/legacy-ml/gcc/2008-07/msg00025.html
-> https://github.com/cpredef/predef/blob/master/Compilers.md

which is precisely what we are using __clang__ for in this 
case. so that is both fortunate, and unfortunate, because
this version is more opaque than the first version imo.

#if defined(__clang__)
#define NOWARN(warnoption, ...)                     \
    DO_PRAGMA(clang diagnostic push)                \
    DO_PRAGMA(clang diagnostic ignored #warnoption) \
    __VA_ARGS__                                     \
    DO_PRAGMA(clang diagnostic pop)
#endif
*/

/**
 * @brief Disables the given warning around the block of code passed to the macro
 * 
 * called like:
 * 
 * NOWARN(-Wthe-warning,
 * whatever-code-you-want
 * )
 * 
 * NOTE 9/15/2022
 * Turns out that this version of the macro is redundant as clang supports 
 * both clang diagnostic push
 * and  GCC diagnostic push
 * so when compiling with clang we get the 
 * -Wmacro-redefined warning because clang defines
 * both __clang__ and __GNUC__,
 *
 * and on that note, there is no __XXXX___ which is defined if
 * and only if GCC is the compiler currently being used.
 * see:
 * -> https://gcc.gnu.org/legacy-ml/gcc/2008-07/msg00025.html
 * -> https://github.com/cpredef/predef/blob/master/Compilers.md
 * 
 * which is precisely what we are using __clang__ for in this 
 * case. so that is both fortunate, and unfortunate, because
 * this version is more opaque than the first version imo.
 * in that it just works for when compiled with GCC or clang.
 *
 * #if defined(__clang__)
 * #define NOWARN(warnoption, ...)                     \
 *   DO_PRAGMA(clang diagnostic push)                \
 *   DO_PRAGMA(clang diagnostic ignored #warnoption) \
 *   __VA_ARGS__                                     \
 *   DO_PRAGMA(clang diagnostic pop)
 * #endif
 * 
 */
#if defined(__GNUC__)
#define NOWARN(warnoption, ...)                   \
    DO_PRAGMA(GCC diagnostic push)                \
    DO_PRAGMA(GCC diagnostic ignored #warnoption) \
    __VA_ARGS__                                   \
    DO_PRAGMA(GCC diagnostic pop)             
#endif