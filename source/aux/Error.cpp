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

#include "aux/Error.h"

#include <sstream>

#include "support/FatalError.h"

namespace pink {
Error::Error(Error::Code code, Location location, std::string_view description)
    : code(code),
      location(location),
      text(description) {}

Error::Error(std::errc errc, Location location, std::string_view description)
    : code(Error::Code::None),
      location(location),
      text(description) {
  if (errc == std::errc::result_out_of_range) {
    code = Error::Code::IntegerOutOfBounds;
  }

  auto              code = std::make_error_code(errc);
  std::stringstream buffer;
  buffer << "std::error_code [" << code << "], description: [" << text << "]\n";

  FatalError(buffer.str());
}

auto Error::ToString(std::string_view bad_source) const -> std::string {
  std::stringstream stream;
  Print(stream, bad_source);
  return stream.str();
}

auto Error::Print(std::ostream &out, std::string_view bad_source) const
    -> std::ostream & {
  out << CodeToErrText(code) << ": " << text << "\n";
  out << bad_source << "\n";
  for (std::size_t i = 0; i < bad_source.size(); i++) {
    if ((i < location.firstColumn) || (i >= location.lastColumn)) {
      out << " ";
    } else {
      out << "^";
    }
  }
  out << "\n";
  return out;
}

constexpr auto Error::CodeToErrText(Error::Code code) -> const char * {
  switch (code) {
  case Error::Code::None:
    return "Default Error, this should not be printed";
  // syntax error descriptions
  case Error::Code::EndOfFile:
    return "End of source file encountered";
  case Error::Code::MissingSemicolon:
    return "Syntax Error: Expected ';'";
  case Error::Code::MissingLParen:
    return "Syntax Error: Expected '('";
  case Error::Code::MissingRParen:
    return "Syntax Error: Expected ')'";
  case Error::Code::MissingLBrace:
    return "Syntax Error: Expected '{'";
  case Error::Code::MissingRBrace:
    return "Syntax Error: Expected '}'";
  case Error::Code::MissingRBracket:
    return "Syntax Error: Expected ']'";
  case Error::Code::MissingVar:
    return "Syntax Error: Expected 'var'";
  case Error::Code::MissingBindId:
    return "Syntax Error: Expected identifier for bind expression";
  case Error::Code::MissingBindColonEq:
    return "Syntax Error: Missing ':=' within bind expression";
  case Error::Code::MissingFn:
    return "Syntax Error: Missing 'fn'";
  case Error::Code::MissingFnName:
    return "Syntax Error: Missing function name";
  case Error::Code::MissingArgName:
    return "Syntax Error: Missing argument name";
  case Error::Code::MissingArgColon:
    return "Syntax Error: Expected ':'";
  case Error::Code::MissingArgType:
    return "Syntax Error: Missing type annotation for argument";
  case Error::Code::MissingArraySemicolon:
    return "Syntax Error: Missing ';' in array type";
  case Error::Code::MissingArrayNum:
    return "Syntax Error: Missing quantity in array type";
  case Error::Code::MissingIf:
    return "Syntax Error: Missing 'if' in conditional expression";
  case Error::Code::MissingThen:
    return "Syntax Error: Missing 'then' in conditional expression";
  case Error::Code::MissingElse:
    return "Syntax Error: Missing 'else' in conditional expression";
  case Error::Code::MissingWhile:
    return "Syntax Error: Missing 'while' in while expression";
  case Error::Code::MissingDo:
    return "Syntax Error: Missing 'do' in while expression";
  case Error::Code::UnknownBinop:
    return "Syntax Error: Unknown binary operator";
  case Error::Code::UnknownUnop:
    return "Syntax Error: Unknown unary operator";
  case Error::Code::UnknownBasicToken:
    return "Syntax Error: Unknown basic token. expected to parse a term";
  case Error::Code::UnknownTypeToken:
    return "Syntax Error: Unknown type token. expected to parse a type";
  case Error::Code::BadTopLevelExpression:
    return "Syntax Error: Bad top level expression";

  // type errors
  case Error::Code::TypeCannotBeCalled:
    return "Type Error: Provided type cannot be called";
  case Error::Code::ArgNumMismatch:
    return "Type Error: Incorrect amount of arguments provided for the given "
           "function";
  case Error::Code::ArgTypeMismatch:
    return "Type Error: Provided argument type does not match functions "
           "argument type";
  case Error::Code::AssigneeTypeMismatch:
    return "Type Error: Assigned type does not match assignee type";
  case Error::Code::NameNotBoundInScope:
    return "Type Error: Name not bound within this scope";
  case Error::Code::NameAlreadyBoundInScope:
    return "Type Error: Name is already bound within this scope";
  case Error::Code::ArrayMemberTypeMismatch:
    return "Type Error: Array element type mismatch";
  case Error::Code::ArraySizeMismatch:
    return "Type Error: Array size mismatch";
  case Error::Code::TupleSizeMismatch:
    return "Type Error: Tuple size mismatch";
  case Error::Code::TupleElementMismatch:
    return "Type Error: Tuple element type mismatch";
  case Error::Code::CondTestExprTypeMismatch:
    return "Type Error: IfThenElse expression's test expression must have "
           "type Boolean";
  case Error::Code::CondBodyExprTypeMismatch:
    return "Type Error: IfThenElse expression's body expressions must have "
           "identical type";
  case Error::Code::WhileTestTypeMismatch:
    return "Type Error: While loop's test expression must have type [Boolean]";
  case Error::Code::DotLeftIsNotATuple:
    return "Type Error: Dot operator's right hand side must be a tuple";
  case Error::Code::DotRightIsNotAnInt:
    return "Type Error: Dot operator's left hand side must be an integer "
           "literal when right is a tuple";
  case Error::Code::DotIndexOutOfRange:
    return "Type Error: Index into tuple is larger than the tuple itself";
  case Error::Code::SubscriptLeftIsNotSubscriptable:
    return "Type Error: Cannot apply Subscript operation to given left hand "
           "side";
  case Error::Code::SubscriptRightIsNotAnIndex:
    return "Type Error: Cannot use argument to Subscript operation as an Index";
  case Error::Code::TypeSubstitutionInvalid:
    return "Type Error: Type substitution invalid";

  // semantic errors
  case Error::Code::OutOfBounds:
    return "Semantic Error: Index out of bounds";
  case Error::Code::ValueCannotBeAssigned:
    return "Semantic Error: Left side cannot be assigned";
  case Error::Code::NonConstGlobalInit:
    return "Semantic Error: Global variables must have a constant initializer";
  case Error::Code::NonConstArrayInit:
    return "Semantic Error: Arrays must have constant initializers";
  case Error::Code::NonConstTupleInit:
    return "Semantic Error: Tuples must have constant initializers";
  case Error::Code::CannotTakeAddressOfLiteral:
    return "Semantic Error: Cannot take the address of a literal value";
  case Error::Code::CannotDereferenceLiteral:
    return "Semantic Error: Cannot dereference a non-pointer value";
  case Error::Code::CannotCastToType:
    return "Semantic Error: Cannot cast value to the target type";
  case Error::Code::CannotCastFromType:
    return "Semantic Error: Cannot cast from value type";
  default:
    return "Unknown Error Code";
  }
}
} // namespace pink
