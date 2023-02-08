/**
 * @file Error.h
 * @brief Header for class Error
 * @version 0.1
 *
 */
#pragma once
#include <iostream>
#include <string>

#include "aux/Location.h"

namespace pink {

/**
 * @brief This class represents an instance of an Error within the compiler
 *        Errors can be Syntactic, Type, or Semantic.
 */
class Error {
public:
  /**
   * @brief This enum is used to uniquely identify each kind of error possible
   * within the compiler. This is so we may potentially catch and handle
   * particular errors.
   *
   */
  enum Code {
    None, // for building no arg errors which are needed to declare
          // an error term that may be overwritten in a lower scope.

    // syntax errors
    EndOfFile,
    MissingSemicolon,
    MissingLParen,
    MissingRParen,
    MissingLBrace,
    MissingRBrace,
    MissingRBracket,
    MissingLessThan,
    MissingGreaterThan,
    MissingVar,
    MissingBindId,
    MissingBindColonEq,
    MissingFn,
    MissingFnName,
    MissingArgName,
    MissingArgType,
    MissingArgColon,
    MissingArraySemicolon,
    MissingArrayNum,
    MissingIf,
    MissingThen,
    MissingElse,
    MissingWhile,
    MissingDo,
    UnknownBinop,
    UnknownUnop,
    UnknownBasicToken,
    UnknownTypeToken,
    BadTopLevelExpression,

    // type errors
    TypeCannotBeCalled,
    ArgNumMismatch,
    ArgTypeMismatch,
    AssigneeTypeMismatch,
    NameNotBoundInScope,
    NameAlreadyBoundInScope,
    ArrayMemberTypeMismatch,
    CondTestExprTypeMismatch,
    CondBodyExprTypeMismatch,
    WhileTestTypeMismatch,
    DotLeftIsNotATuple,
    DotRightIsNotAnInt,
    DotIndexOutOfRange,
    SubscriptLeftIsNotSubscriptable,
    SubscriptRightIsNotAnIndex,

    // semantic errors
    OutOfBounds,
    IntegerOutOfBounds,
    ValueCannotBeAssigned,
    NonConstGlobalInit,
    NonConstArrayInit,
    NonConstTupleInit,
    CannotTakeAddressOfLiteral,
    CannotDereferenceLiteral,
    CannotCastToType,
    CannotCastFromType,
  };

  /**
   * @brief The error code of this particular error
   *
   */
  Code code;

  /**
   * @brief the syntactic location of this particular error
   *
   */
  Location location;

  /**
   * @brief the text representing information about the error generated at the
   * time of the errors creation.
   *
   * \note it would be really nice if we could make this member a std::string_view.
   * for both time and space efficiency. However, there are instances where we 
   * construct more elaborate error messages using strings constructed at the 
   * point where the error is found. In this case we must hold the dynamic 
   * allocation of the string, and not simply a reference to text held somewhere 
   * else. if we could store the strings elsewhere, then the error structure itself 
   * could be made smaller and thus faster to return. But the issue arises 
   * precisely because we must store the string computed for it to be processed 
   * higher up the call stack. (when the error is handled.)
   */
  std::string text;

  /**
   * @brief Maps between error codes and their corresponding default error
   * messages.
   *
   * These messages are usually not informative enough about the error on their
   * own to truly help the user locate and correct the error, they are simply
   * the common parts of each error message, stored seperately from any
   * particular error message to help reduce the size of any particular error
   * as it is passed around. (it would be faster to only store a reference
   * to the complete message in some lookup table, but it would really only
   * reduce some stack traffic, as we *want* highly context dependent
   * information only knowable at the particular place the error was generated
   * to be propagated out to help compose the error message. )
   *
   * \warning it is unsafe to write to any message returned from this routine,
   * as all messages are globally stored c-strings
   *
   * @param code the code to covert
   * @return const char* the message corresponding to the error code
   */
  [[nodiscard]] constexpr static auto CodeToErrText(Code code) -> const char *;

  /**
   * @brief Construct a new Error, with error code None, Location(0, 0, 0, 0),
   * and empty text
   *
   */
  Error() : code(Error::Code::None) {}

  /**
   * @brief Destroy the Error object
   *
   */
  ~Error() = default;

  /**
   * @brief Copy Construct an Error, as a copy of the passed error
   *
   * @param other
   */
  Error(const Error &other) = default;

  /**
   * @brief Move Construct an Error
   *
   */
  Error(Error &&other) = default;

  /**
   * @brief Construct a new Error.
   *
   * @param code the code this error represents.
   * @param location the textual location of this error.
   * @param text contextual text relevant to this particular error.
   */
  Error(Code code, Location location, std::string text = "");

  /**
   * @brief Copy Assignment.
   *
   * @param other the other error to copy from.
   * @return Error& this error.
   */
  auto operator=(const Error &other) -> Error & = default;

  /**
   * @brief Move Assignment
   *
   * @param other the other error to move from
   * @return Error& this error
   */
  auto operator=(Error &&other) -> Error & = default;

  /**
   * @brief convert this error to a string representing the error
   *
   * @param bad_source the line of source causing the error
   * @return std::string
   */
  [[nodiscard]] auto ToString(std::string_view bad_source) const -> std::string;

  /**
   * @brief print this particular error to the given output stream.
   *
   * @param out the output stream to write to.
   * @param txt the line of text which created this error
   * @return std::ostream& the output stream given
   */
  auto Print(std::ostream &out, std::string_view bad_source) const
      -> std::ostream &;
};

/**
 * @brief Writes dsc to stderr and then calls exit(1).
 *
 * useful when the error in question cannot be recovered from.
 * or it is better for the developer of the program to fix the
 * error, not a programmer writing in the langauge.
 *
 * \todo it would be a performance win to enclose calls to
 * FatalError with preprocessor guards so that they are not
 * compiled into release builds. (This relys on the assumption
 * that the FatalError call in question is never encountered
 * during operation of the compiler, and is only there to dynamically
 * assert some property) ((why not use assert? because sometimes it's
 * nice to write out a bit of runtime information in the error message,
 * which is not easy to do with an assert))
 *
 *
 * @param dsc a description of the fatal error
 * @param file the file this error occured within
 * @param line the line this error occured within
 */
[[noreturn]] void FatalError(const char *dsc, const char *file, size_t line);

/**
 * @copydoc FatalError(const char* dsc, const char * file, size_t line)
 */
[[noreturn]] void FatalError(const std::string &dsc, const char *file,
                             size_t line);

} // namespace pink
