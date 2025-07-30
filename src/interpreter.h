#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"

// The main entry point for the interpreter.
// It will execute the program represented by the AST.
void interpret(ASTNode* program);

#endif // INTERPRETER_H