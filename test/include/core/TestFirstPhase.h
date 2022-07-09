#pragma once 
#include <iostream>

// Tests that the parser returns Ast's that can be typed correctly.
// essentially combined unit tests for the five main components of the compiler, (lexer/parser, Ast, Aux, Ops, Types) 
// up to codegeneration. TestSecondPhase will test against the generated code, when implemented.
bool TestFirstPhase(std::ostream& out);
