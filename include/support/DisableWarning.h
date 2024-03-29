// Copyright (C) 2023 cadence
// 
// This file is part of pink.
// 
// pink is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// pink is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with pink.  If not, see <http://www.gnu.org/licenses/>.

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
// Outline of this code retrieved from
// https://stackoverflow.com/questions/48426484/concise-way-to-disable-specific-warning-instances-in-clang

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
// I agree with the sentiment that macros are to
// be avoided if at all possible. However given the semantics
// that NOWARN is implementing, I am unsure how to
// accomplish something similar using even variadic
// constexpr templates. because the code being repeated
// is not knowable at the time when the macro is defined.
// instead, whatever code which we want to turn the warning
// off for is spliced in. This is unfortunately textual macro
// specific semantics as far as I am aware.

#if defined(__GNUC__)
#define PRAGMA(x) _Pragma(#x)
#else
#define PRAGAM(x)
#endif
/*
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
 * which is precisely what we were using __clang__ for in this
 * case. so that is both fortunate, and unfortunate, because
 * this version is more opaque than the first version imo.
 * in that it just works when compiled with GCC or clang.
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
/** @def NOWARN(warnoption, ...)
 * @brief Disables the given warning around the block of code passed to the
 * macro as it's __VA_ARGS__
 *
 * called like:
 *
 * NOWARN("-Wthe-warning",
 * valid_cpp_code;
 * )
 */
#if defined(__GNUC__)
#define NOWARN(warnoption, ...)                                                \
  PRAGMA(GCC diagnostic push)                                                  \
  PRAGMA(GCC diagnostic ignored warnoption)                                    \
  __VA_ARGS__                                                                  \
  PRAGMA(GCC diagnostic pop)
#else
#define NOWARN(warnoption, ...) __VA_ARGS__
#endif

// NOLINTEND(cppcoreguidelines-macro-usage)