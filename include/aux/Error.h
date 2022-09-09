#pragma once
#include <string>
#include <iostream>

#include "aux/Location.h"

namespace pink {

  class Error {
    public:
      enum Code 
      {
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
        MissingFn,
        MissingFnName,
        MissingArgName,
        MissingArgType,
        MissingArgColon,
        MissingArrayX,
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
        DotLeftIsNotAStruct,
        DotRightIsNotAnInt,
        DotIndexOutOfRange,

        // semantic errors
        ValueCannotBeAssigned,
        NonConstGlobalInit,
        NonConstArrayInit,
        NonConstTupleInit,
        CannotTakeAddressOfLiteral,
        CannotDereferenceLiteral,
        CannotCastToType,
        CannotCastFromType,
      };

      Code     code;
      Location loc;
      // So, here we hold a std::string to allow for customization of 
      // the error message at the point of creation, allowing for more 
      // contextual information to be returned about the error.
      // this seems like a necessary feature for informative error 
      // messages. however, passing a std::string around through 
      // return values is more expensive than not passing a std string 
      // around. A pointer would be cheaper, but how could we store a 
      // pointer to memory that would live beyond the point of creation?
      // it would have to be stored somewhere else.
      std::string text;
      
      const char* CodeToErrText(Code code);
 
      Error();
      Error(const Error& other);
      Error(Code code, Location l, std::string text = "");

      Error& operator=(const Error& other);


      std::string ToString(const char* txt);
      std::string ToString(std::string& txt);

      std::ostream& Print(std::ostream& out, std::string& txt);
      std::ostream& Print(std::ostream& out, const char* txt);
  };

  void FatalError(const char* dsc, const char* file, size_t line);
  void FatalError(std::string dsc, const char* file, size_t line);

}
