#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"
typedef struct {
    Token current;
    Token previous;
    int had_error;
    int panic_mode;
} Parser;

extern Parser parser;
// The main function of the parser. It returns the root of the FULL AST.
// Note: For simplicity, we'll return the AST of the last statement for now.
// A better approach would be a list of statements.
ASTNode* parse();
void consume(TokenType type, const char* message);
#endif // PARSER_H