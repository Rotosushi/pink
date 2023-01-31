#include "aux/TestError.h"
#include "Test.h"

#include "aux/Error.h"

bool TestError(std::ostream &out) {
  bool        result = true;
  std::string name   = "pink::Error";
  TestHeader(out, name);

  srand(time(nullptr)); // seed random number generation

  auto Testlet = [](std::ostream &out, const char *testtxt,
                    pink::Error::Code code, std::string &errtxt) -> bool {
    bool result = true;

    size_t len = errtxt.size();

    size_t x = rand() % len,
           y = rand() % len; // two random positions within the given text

    // the first column must appear before the last column,
    // so we assign it the smaller value between x and y.
    size_t first_column = x <= y ? x : y;
    size_t last_column  = x <= y ? y : x;

    pink::Location errloc(0, first_column, 0, last_column);

    pink::Error error(code, errloc);

    std::string errstr = error.ToString(errtxt);

    result &= Test(out, testtxt, errstr.empty());

    return result;
  };

  std::string errtxt = "some erroneous input text";

  // syntax error codes
  result &= Testlet(out, "Error::Code::None", pink::Error::Code::None, errtxt);
  result &= Testlet(out, "Error::Code::EndOfFile", pink::Error::Code::EndOfFile,
                    errtxt);
  result &= Testlet(out, "Error::Code::MissingSemicolon",
                    pink::Error::Code::MissingSemicolon, errtxt);
  result &= Testlet(out, "Error::Code::MissingLParen",
                    pink::Error::Code::MissingLParen, errtxt);
  result &= Testlet(out, "Error::Code::MissingRParen",
                    pink::Error::Code::MissingRParen, errtxt);
  result &= Testlet(out, "Error::Code::MissingLBrace",
                    pink::Error::Code::MissingLBrace, errtxt);
  result &= Testlet(out, "Error::Code::MissingRBrace",
                    pink::Error::Code::MissingRBrace, errtxt);
  result &= Testlet(out, "Error::Code::MissingFn", pink::Error::Code::MissingFn,
                    errtxt);
  result &= Testlet(out, "Error::Code::MissingFnName",
                    pink::Error::Code::MissingFnName, errtxt);
  result &= Testlet(out, "Error::Code::MissingArgName",
                    pink::Error::Code::MissingArgName, errtxt);
  result &= Testlet(out, "Error::Code::MissingArgType",
                    pink::Error::Code::MissingArgType, errtxt);
  result &= Testlet(out, "Error::Code::MissingArgColon",
                    pink::Error::Code::MissingArgColon, errtxt);
  result &= Testlet(out, "Error::Code::UnknownBinop",
                    pink::Error::Code::UnknownBinop, errtxt);
  result &= Testlet(out, "Error::Code::UnknownUnop",
                    pink::Error::Code::UnknownUnop, errtxt);
  result &= Testlet(out, "Error::Code::UnknownBasicToken",
                    pink::Error::Code::UnknownBasicToken, errtxt);

  // type error codes
  result &= Testlet(out, "Error::Code::TypeCannotBeCalled",
                    pink::Error::Code::TypeCannotBeCalled, errtxt);
  result &= Testlet(out, "Error::Code::ArgNumMismatch",
                    pink::Error::Code::ArgNumMismatch, errtxt);
  result &= Testlet(out, "Error::Code::ArgTypeMismatch",
                    pink::Error::Code::ArgTypeMismatch, errtxt);
  result &= Testlet(out, "Error::Code::AssigneeTypeMismatch",
                    pink::Error::Code::AssigneeTypeMismatch, errtxt);
  result &= Testlet(out, "Error::Code::NameNotBoundInScope",
                    pink::Error::Code::NameNotBoundInScope, errtxt);
  result &= Testlet(out, "Error::Code::NameAlreadyBoundInScope",
                    pink::Error::Code::NameAlreadyBoundInScope, errtxt);

  // semantic error codes
  result &= Testlet(out, "Error::Code::ValueCannotBeAssigned",
                    pink::Error::Code::ValueCannotBeAssigned, errtxt);
  result &= Testlet(out, "Error::Code::NonConstGlobalInit",
                    pink::Error::Code::NonConstGlobalInit, errtxt);

  return TestFooter(out, name, result);
}
