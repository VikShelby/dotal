// =================================================================
// ==  REPLACE THE ENTIRE CONTENTS OF src/parser.c WITH THIS CODE ==
// =================================================================

#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "lexer.h"
#include "ast.h"

// At the top of src/parser.c, where the variable used to be:
Parser parser; // The actual variable definition, without the 'extern'

typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT,
    PREC_OR,
    PREC_AND,
    PREC_EQUALITY,
    PREC_COMPARISON,
    PREC_TERM,
    PREC_FACTOR,
    PREC_UNARY,
    PREC_CALL,
    PREC_PRIMARY
} Precedence;

typedef struct {
    ASTNode* (*prefix)(void);
    ASTNode* (*infix)(ASTNode* left);
    Precedence precedence;
} ParseRule;

// --- Forward Declarations ---
static ASTNode* unary();
static ASTNode* expression();
static ASTNode* declaration();
static ASTNode* logical_(ASTNode* left);
static ASTNode* for_statement();
static ASTNode* block();
static ASTNode* if_statement();
static ASTNode* while_statement();
static ASTNode* print_statement();
static ASTNode* var_declaration();
static ASTNode* expression_statement();
static ASTNode* parse_precedence(Precedence precedence);
static ASTNode* binary(ASTNode* left);
static ASTNode* number();
static ASTNode* string_();
static ASTNode* grouping();
static ASTNode* literal();
static ASTNode* variable();
static ASTNode* assignment(ASTNode* left);
static ASTNode* function_declaration();
static ASTNode* call(ASTNode* callee);
static ASTNode* return_statement();
// --- The Parsing Table ---
ParseRule rules[] = {
    [TOKEN_VAR]           = { NULL,     NULL,       PREC_NONE },
    [TOKEN_IF]            = { NULL,     NULL,       PREC_NONE },
    [TOKEN_ELSE]          = { NULL,     NULL,       PREC_NONE },
    [TOKEN_WHILE]         = { NULL,     NULL,       PREC_NONE },
    [TOKEN_FUNCTION]      = { NULL,     NULL,       PREC_NONE },
    [TOKEN_RETURN]        = { NULL,     NULL,       PREC_NONE },
    [TOKEN_PRINT]         = { NULL,     NULL,       PREC_NONE },
    [TOKEN_IDENTIFIER]    = { variable, NULL,       PREC_NONE },
    [TOKEN_NUMBER]        = { number,   NULL,       PREC_NONE },
    [TOKEN_STRING]        = { string_,  NULL,       PREC_NONE },
    [TOKEN_TRUE]          = { literal,  NULL,       PREC_NONE },
    [TOKEN_FALSE]         = { literal,  NULL,       PREC_NONE },
    [TOKEN_EQUAL]         = { NULL,     assignment, PREC_ASSIGNMENT },
    [TOKEN_PLUS]          = { NULL,     binary,     PREC_TERM },
    [TOKEN_MINUS]         = { unary,     binary,     PREC_TERM },
    [TOKEN_STAR]          = { NULL,     binary,     PREC_FACTOR },
    [TOKEN_SLASH]         = { NULL,     binary,     PREC_FACTOR },
    [TOKEN_EQUAL_EQUAL]   = { NULL,     binary,     PREC_EQUALITY },
    [TOKEN_BANG_EQUAL]    = { NULL,     binary,     PREC_EQUALITY },
    [TOKEN_GREATER]       = { NULL,     binary,     PREC_COMPARISON },
    [TOKEN_GREATER_EQUAL] = { NULL,     binary,     PREC_COMPARISON },
    [TOKEN_LESS]          = { NULL,     binary,     PREC_COMPARISON },
    [TOKEN_LESS_EQUAL]    = { NULL,     binary,     PREC_COMPARISON },
    [TOKEN_LPAREN]       = { grouping, call,       PREC_CALL },
    [TOKEN_RPAREN]        = { NULL,     NULL,       PREC_NONE },
    [TOKEN_LBRACE]        = { NULL,     NULL,       PREC_NONE },
    [TOKEN_RBRACE]        = { NULL,     NULL,       PREC_NONE },
    [TOKEN_SEMICOLON]     = { NULL,     NULL,       PREC_NONE },
    [TOKEN_EOF]           = { NULL,     NULL,       PREC_NONE },
    [TOKEN_ERROR]         = { NULL,     NULL,       PREC_NONE },
    [TOKEN_AND]          = { NULL,     logical_,   PREC_AND },  // <-- ADD THIS
    [TOKEN_OR]           = { NULL,     logical_,   PREC_OR },   // <-- ADD THIS
};


// --- Helper Functions ---
static void error_at(Token* token, const char* message) {
    if (parser.panic_mode) return;
    parser.panic_mode = 1;
    fprintf(stderr, "[line %d] Error at '%.*s': %s\n", token->line, token->length, token->start, message);
    parser.had_error = 1;
}
// In parser.c, add this new function
static ASTNode* unary() {
    Token operator = parser.previous;
    // To parse the operand, we recursively call parse_precedence
    // with the precedence of a unary operator.
    ASTNode* right = parse_precedence(PREC_UNARY);
    return create_unary_node(operator, right);
}
static void advance() {
    parser.previous = parser.current;
    for (;;) {
        parser.current = scan_token();
        if (parser.current.type != TOKEN_ERROR) break;
        error_at(&parser.current, parser.current.start);
    }
}

static int check(TokenType type) { return parser.current.type == type; }
static int match(TokenType type) { if (!check(type)) return 0; advance(); return 1; }
void consume(TokenType type, const char* message) { if (check(type)) { advance(); return; } error_at(&parser.current, message); }


// --- Grammar Implementation ---
static ASTNode* grouping() {
    // The '(' has already been consumed.
    // Now we parse the expression inside the parentheses.
    ASTNode* expr = expression();
    
    // After the expression, we must find a closing ')'.
    consume(TOKEN_RPAREN, "Expect ')' after expression.");
    
    // The result is just the expression that was inside.
    return expr;
}
static ASTNode* parse_precedence(Precedence precedence) {
    advance();
    ASTNode* (*prefix_rule)(void) = rules[parser.previous.type].prefix;
    if (prefix_rule == NULL) {
        error_at(&parser.previous, "Expect expression.");
        return NULL;
    }
    ASTNode* left_node = prefix_rule();
    while (precedence <= rules[parser.current.type].precedence) {
        advance();
        ASTNode* (*infix_rule)(ASTNode*) = rules[parser.previous.type].infix;
        left_node = infix_rule(left_node);
    }
    return left_node;
}

static ASTNode* expression() {
    return parse_precedence(PREC_ASSIGNMENT);
}

static ASTNode* binary(ASTNode* left) {
    Token operator = parser.previous;
    ParseRule* rule = &rules[operator.type];
    ASTNode* right = parse_precedence((Precedence)(rule->precedence + 1));
    return create_binary_expr_node(left, operator, right);
}
static ASTNode* logical_(ASTNode* left) {
    Token operator = parser.previous;
    ParseRule* rule = &rules[operator.type];
    ASTNode* right = parse_precedence((Precedence)(rule->precedence + 1));
    return create_logical_node(left, operator, right);
}
static ASTNode* number() {
    return create_literal_node(parser.previous);
}

static ASTNode* string_() {
    return create_literal_node(parser.previous);
}

static ASTNode* literal() {
    return create_literal_node(parser.previous);
}

static ASTNode* variable() {
    return create_variable_expr_node(parser.previous);
}

static ASTNode* assignment(ASTNode* left) {
    if (left->type == NODE_VARIABLE_EXPRESSION) {
        Token name = ((VariableExprNode*)left)->name;
        ASTNode* value = parse_precedence(PREC_ASSIGNMENT);
        return create_assign_node(name, value);
    }
    error_at(&parser.previous, "Invalid assignment target.");
    return NULL;
}

static ASTNode* expression_statement() {
    ASTNode* expr = expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after expression.");
    // An assignment is an expression, but it's used as a statement here.
    // So we wrap it, or just return it as is. Let's return as is.
    return expr;
}

static ASTNode* print_statement() {
    ASTNode* value = expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after value.");
    return create_print_node(value);
}

static ASTNode* var_declaration() {
    consume(TOKEN_IDENTIFIER, "Expect variable name.");
    Token name = parser.previous;
    ASTNode* initializer = NULL;
    if (match(TOKEN_EQUAL)) {
        initializer = expression();
    }
    consume(TOKEN_SEMICOLON, "Expect ';' after variable declaration.");
    return create_var_decl_node(name, initializer);
}

static ASTNode* block() {
    ProgramNode* block_node = (ProgramNode*)create_program_node();
    while (!check(TOKEN_RBRACE) && !check(TOKEN_EOF)) {
        add_statement_to_program(block_node, declaration());
    }
    consume(TOKEN_RBRACE, "Expect '}' after block.");
    return (ASTNode*)block_node;
}

static ASTNode* if_statement() {
    consume(TOKEN_LPAREN, "Expect '(' after 'nese'.");
    ASTNode* condition = expression();
    consume(TOKEN_RPAREN, "Expect ')' after condition.");
    consume(TOKEN_LBRACE, "Expect '{' before nese branch.");
    ASTNode* then_branch = block();
    ASTNode* else_branch = NULL;
    if (match(TOKEN_ELSE)) {
        consume(TOKEN_LBRACE, "Expect '{' before perndryshe branch.");
        else_branch = block();
    }
    return create_if_node(condition, then_branch, else_branch);
}

static ASTNode* while_statement() {
    consume(TOKEN_LPAREN, "Expect '(' after 'derisa'.");
    ASTNode* condition = expression();
    consume(TOKEN_RPAREN, "Expect ')' after condition.");
    consume(TOKEN_LBRACE, "Expect '{' before derisa body.");
    ASTNode* body = block();
    return create_while_node(condition, body);
}

static ASTNode* statement() {
    if (match(TOKEN_PRINT)) return print_statement();
    if (match(TOKEN_IF)) return if_statement();
    if (match(TOKEN_WHILE)) return while_statement();
    if (match(TOKEN_RETURN)) return return_statement();
    if (match(TOKEN_FOR)) return for_statement();
    return expression_statement();
}
static ASTNode* for_statement() {
    consume(TOKEN_LPAREN, "Expect '(' after 'per'.");

    // 1. Parse the Initializer
    ASTNode* initializer;
    if (match(TOKEN_SEMICOLON)) {
        initializer = NULL; // No initializer
    } else if (match(TOKEN_VAR)) {
        initializer = var_declaration();
    } else {
        initializer = expression_statement();
    }

    // 2. Parse the Condition
    ASTNode* condition = NULL;
    if (!check(TOKEN_SEMICOLON)) {
        condition = expression();
    }
    consume(TOKEN_SEMICOLON, "Expect ';' after loop condition.");

    // 3. Parse the Increment
    ASTNode* increment = NULL;
    if (!check(TOKEN_RPAREN)) {
        increment = expression();
    }
    consume(TOKEN_RPAREN, "Expect ')' after for clauses.");

    // 4. Parse the Body
    consume(TOKEN_LBRACE, "Expect '{' before per body.");
    ASTNode* body = block();

    // --- 5. De-sugaring: Build the While Loop AST ---

    // If there's an increment, add it to the end of the main body block.
    if (increment != NULL) {
        // The body must be a ProgramNode (a block) because we created it with block()
        add_statement_to_program((ProgramNode*)body, increment);
    }

    // If there's no condition, make it 'vertete' to loop forever.
    if (condition == NULL) {
        // We simulate a 'true' literal by creating a literal node with the 'vertete' token
        Token true_token = {.type = TOKEN_TRUE}; // A synthetic token
        condition = create_literal_node(true_token);
    }
    
    // Create the main 'derisa' (while) loop.
    ASTNode* while_loop = create_while_node(condition, body);

    // If there was an initializer, wrap the while loop in a new block with it.
    if (initializer != NULL) {
        ProgramNode* outer_block = (ProgramNode*)create_program_node();
        add_statement_to_program(outer_block, initializer);
        add_statement_to_program(outer_block, while_loop);
        return (ASTNode*)outer_block;
    }

    // If no initializer, just return the while loop itself.
    return while_loop;
}
// REPLACE the function_declaration function
static ASTNode* function_declaration() {
    consume(TOKEN_IDENTIFIER, "Expect function name.");
    Token name = parser.previous;
    consume(TOKEN_LPAREN, "Expect '(' after function name.");
  // In src/parser.c -> function_declaration
// ... after consume(TOKEN_LPAREN, ...);


Token* params = NULL;
int arity = 0;
if (!check(TOKEN_RPAREN)) {
    do {
        if (arity == 255) {
            error_at(&parser.current, "Can't have more than 255 parameters.");
        }
        consume(TOKEN_IDENTIFIER, "Expect parameter name.");
        
        arity++;
        params = (Token*)realloc(params, sizeof(Token) * arity);
        params[arity - 1] = parser.previous;
    } while (match(TOKEN_COMMA));
}
consume(TOKEN_RPAREN, "Expect ')' after parameters.");
// ... rest of function is the same ...
    
    consume(TOKEN_LBRACE, "Expect '{' before function body.");
    ASTNode* body = block();
    
    return create_function_decl_node(name, params, arity, body);
}

// REPLACE the call function
static ASTNode* call(ASTNode* callee) {
    ASTNode** arguments = NULL;
    int arg_count = 0;

    if (!check(TOKEN_RPAREN)) {
        do {
            arg_count++;
            arguments = (ASTNode**)realloc(arguments, sizeof(ASTNode*) * arg_count);
            arguments[arg_count - 1] = expression();
        } while (match(TOKEN_COMMA));
    }

    consume(TOKEN_RPAREN, "Expect ')' after arguments.");
    return create_call_node(callee, arguments, arg_count);
}
static ASTNode* return_statement() {
    Token keyword = parser.previous;
    ASTNode* value = NULL;
    if (!check(TOKEN_SEMICOLON)) {
        value = expression();
    }
    consume(TOKEN_SEMICOLON, "Expect ';' after return value.");
    return create_return_node(keyword, value);
}
static ASTNode* declaration() {
    if (match(TOKEN_FUNCTION)) {
        return function_declaration();
    }
    if (match(TOKEN_VAR)) {
        return var_declaration();
    }
    return statement();
}


// --- Main Parser Function ---
// In src/parser.c
// REPLACE the entire parse() function at the bottom

ASTNode* parse() {
    parser.had_error = 0;
    parser.panic_mode = 0;
    advance();

    // The original, correct code:
    ProgramNode* program = (ProgramNode*)create_program_node();
    while (!match(TOKEN_EOF)) {
        // We are not at the end of the file, so we must be at the beginning of a declaration.
        add_statement_to_program(program, declaration());
    }
    
    return (ASTNode*)program;
}