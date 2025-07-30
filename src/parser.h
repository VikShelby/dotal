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
ASTNode* parse();
void consume(TokenType type, const char* message);
#endif // PARSER_H