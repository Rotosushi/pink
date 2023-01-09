#include "aux/Error.h"

namespace pink {
Error::Error(Error::Code code, Location loc, std::string text)
    : code(code), loc(loc), text(std::move(text)) {}

auto Error::ToString(const char *txt) const -> std::string {
  std::string tmp = txt;
  return ToString(tmp);
}

auto Error::ToString(std::string &errtxt) const -> std::string {
  std::string result;

  result += CodeToErrText(code) + std::string(": ") + std::string(text) + "\n";
  result += errtxt + "\n";

  for (size_t i = 0; i < errtxt.size(); i++) {
    if ((i < loc.firstColumn) || (i >= loc.lastColumn)) {
      result += " ";
    } else {
      result += "^";
    }
  }
  result += "\n";

  return result;
}

auto Error::Print(std::ostream &out, std::string &txt) const -> std::ostream & {
  out << ToString(txt);
  return out;
}

auto Error::Print(std::ostream &out, const char *txt) const -> std::ostream & {
  out << ToString(txt);
  return out;
}

constexpr auto Error::CodeToErrText(Error::Code code) -> const char * {
  switch (code) {
  case Error::Code::None:
    return "Default Error, this should not be printed";
  // syntax error descriptions
  case Error::Code::EndOfFile:
    return "Syntax Error: End Of Input";
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
    return "Syntax Error: Missind 'do' is while expression";
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
    return "Type Error: Array member does not have the same type as other "
           "array members";
  case Error::Code::CondTestExprTypeMismatch:
    return "Type Error: Conditional expression's test expression must have "
           "type Boolean";
  case Error::Code::CondBodyExprTypeMismatch:
    return "Type Error: Conditional expression's body expressions must have "
           "identical type";
  case Error::Code::WhileTestTypeMismatch:
    return "Type Error: While loop's test expression must have type Boolean";
  case Error::Code::DotLeftIsNotATuple:
    return "Type Error: Dot operator's right hand side must be a tuple";
  case Error::Code::DotRightIsNotAnInt:
    return "Type Error: Dot operator's left hand side must be an integer when "
           "right is a tuple";
  case Error::Code::DotIndexOutOfRange:
    return "Type Error: Index into tuple is larger than the tuple itself";
  case Error::Code::SubscriptLeftIsNotSubscriptable:
    return "Type Error: Cannot apply Subscript operation to given left hand "
           "side";
  case Error::Code::SubscriptRightIsNotAnIndex:
    return "Type Error: Cannot use argument to Subscript operation as an Index";

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

void FatalError(const char *dsc, const char *file, size_t line) {
  std::cerr << "Fatal Error in file: " << file << ", line: " << line << ": "
            << dsc << std::endl;
  exit(1);
}

void FatalError(const std::string &dsc, const char *file, size_t line) {
  std::cerr << "Fatal Error in file: " << file << ", line: " << line << ": "
            << dsc << std::endl;
  exit(1);
}
} // namespace pink
