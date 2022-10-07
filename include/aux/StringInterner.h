/**
 * @file StringInterner.h
 * @brief Header for class StringInterner
 * @version 0.1
 *
 */
#pragma once
#include <string>
#include <utility> // pair

#include <llvm/ADT/StringSet.h>

namespace pink {
/**
 * @brief represents a string which has been interned by the string interner.
 *
 */
using InternedString = const char *;

/**
 * @brief Wrapper around a set of strings.
 *
 * only one instance of a given sequence of characters will be stored
 * at any given time. thus, to know if a given sequence of characters
 * is equal to another, we need only compare pointers to said strings.
 * thereby avoiding a call to strcmp for each time we need to know
 * if two strings are equal. Which occurs mainly during symbol lookup,
 * and operator lookup.
 *
 */
class StringInterner {
private:
  /**
   * @brief the set of strings managed by the StringInterner
   *
   * we call this a set in reference to the uniqueness rule
   * of mathematical sets.
   */
  llvm::StringSet<> set;

public:
  /**
   * @brief Construct a new String Interner
   *
   * \warning this must be done only when a new compilation unit is created,
   * as strings held within two distinct StringInterners are not able to be
   * compared via pointer equality.
   */
  StringInterner() = default;

  /**
   * @brief Interns a given string
   *
   * the set will only insert a new string if the string given *is
   * not* already a member of the set. in the case where the string
   * given *is* a member of the set, the set member is returned and
   * the passed value is ignored.
   *
   * @param str the string to intern
   * @return InternedString the InternedString representing the string passed in
   */
  auto Intern(const char *str) -> InternedString;

  /**
   * @copydoc StringInterner::Intern(const char* str)
   */
  auto Intern(std::string &str) -> InternedString;
};

} // namespace pink
