/**
 * @file Error.h
 * @brief Header for class Error
 * @version 0.1
 * 
 */
#pragma once
#include <string>
#include <iostream>

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
       * within the compiler. This is so we may potentially catch and handle particular
       * errors.
       * 
       */
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
        MissingVar,
        MissingBindId,
        MissingBindColonEq,
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

      /**
       * @brief The error code of this particular error
       * 
       */
      Code     code;

      /**
       * @brief the syntactic location of this particular error
       * 
       */
      Location loc;

      /**
       * @brief the text representing information about the error generated at the time of the errors creation.
       * 
       * we hold a std::string to allow for customization of 
       * the error message at the point of creation, allowing for more 
       * contextual information to be returned about the error.
       * this seems like a necessary feature for informative error 
       * messages. however, passing a std::string around through 
       * return values is more expensive than not passing a std string 
       * around. A pointer would be cheaper, but how could we store a 
       * pointer to memory that would live beyond the point of creation?
       * it would have to be stored somewhere else.
       */
      std::string text;
      
      /**
       * @brief Maps between error codes and their corresponding default error messages.
       * 
       * These messages are usually not informative enough about the error on their 
       * own to truly help the user locate and correct the error, they are simply the 
       * common parts of each error message, stored seperately from any particular error
       * message to help reduce the size of any particular error.
       * 
       * \warning it is unsafe to write to any message returned from this routine,
       * as all messages are globally stored c-strings
       * 
       * @param code the code to covert
       * @return const char* the message corresponding to the error code
       */
      const char* CodeToErrText(Code code);
 
      /**
       * @brief Construct a new Error, with error code None, Location(0, 0, 0, 0), and empty text
       * 
       */
      Error();

      /**
       * @brief Construct a new Error, as a copy of the passed error
       * 
       * @param other 
       */
      Error(const Error& other);

      /**
       * @brief Construct a new Error.
       * 
       * @param code the code this error represents.
       * @param l the textual location of this error.
       * @param text contextual text relevant to this particular error. 
       */
      Error(Code code, Location l, std::string text = "");

      /**
       * @brief Assigns this Error to the value of another Error.
       * 
       * @param other the other error to copy from.
       * @return Error& this error.
       */
      Error& operator=(const Error& other);

      /**
       * @brief converts this particular error to a single string of text.
       * 
       * @param txt the line of text which created this error (can be empty)
       * @return std::string the text representing this particular error..
       */
      std::string ToString(const char* txt);
      std::string ToString(std::string& txt);

      /**
       * @brief print this particular error to the given output stream.
       * 
       * @param out the output stream to write to.
       * @param txt the line of text which created this error
       * @return std::ostream& the output stream given
       */
      std::ostream& Print(std::ostream& out, std::string& txt);
      std::ostream& Print(std::ostream& out, const char* txt);
  };

  /**
   * @brief Writes dsc to stderr and then calls exit(1).
   * 
   * useful when the error in question cannot be recovered from.
   * and it is better for the developer of the program to fix the 
   * error, not a programmer writing in the langauge.
   * 
   * \todo FatalError might be better replaced by assert statements.
   * as assert statements do not get compiled into release executables,
   * and presumably a release of the compiler would have most errors 
   * relevant to the developer fixed, so the conditions would never be 
   * met to exit the process in a release build.
   * 
   * @param dsc a description of the fatal error
   * @param file the file this error occured within
   * @param line the line this error occured within
   */
  void FatalError(const char* dsc, const char* file, size_t line);
  void FatalError(std::string dsc, const char* file, size_t line);

}
