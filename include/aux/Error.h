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
    ArraySizeMismatch,
    TupleSizeMismatch,
    TupleElementMismatch,
    CondTestExprTypeMismatch,
    CondBodyExprTypeMismatch,
    WhileTestTypeMismatch,
    DotLeftIsNotATuple,
    DotRightIsNotAnInt,
    DotIndexOutOfRange,
    SubscriptLeftIsNotSubscriptable,
    SubscriptRightIsNotAnIndex,
    TypeSubstitutionInvalid,
    OverloadDoesNotExist,

    // semantic errors
    OutOfBounds,
    IntegerOutOfBounds,
    ValueCannotBeAssigned,
    NonConstGlobalInit,
    NonConstArrayInit,
    NonConstTupleInit,
    CannotTakeAddressOfLiteral,
    CannotDereferenceNonPointer,
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
