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
  enum Code {
    None,

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

  Code        code;
  Location    location;
  std::string text;

  [[nodiscard]] constexpr static auto CodeToErrText(Code code) -> const char *;

  Error()
      : code(Error::Code::None) {}
  Error(Code code, Location location, std::string_view text = "");
  [[noreturn]] Error(std::errc        errc,
                     Location         location,
                     std::string_view text = "");
  ~Error()                                      = default;
  Error(const Error &other)                     = default;
  Error(Error &&other)                          = default;
  auto operator=(const Error &other) -> Error & = default;
  auto operator=(Error &&other) -> Error      & = default;

  [[nodiscard]] auto ToString(std::string_view bad_source) const -> std::string;
  auto               Print(std::ostream &out, std::string_view bad_source) const
      -> std::ostream &;
};
} // namespace pink
