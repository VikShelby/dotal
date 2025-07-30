// =================================================================
// ==  COPY THIS ENTIRE BLOCK INTO YOUR EMPTY src/ast.h FILE    ==
// =================================================================

#ifndef AST_H
#define AST_H

#include "lexer.h" // We need the Token struct

// Define the different kinds of nodes our AST can have
typedef enum {
    NODE_PROGRAM,
    NODE_BINARY_EXPRESSION,
    NODE_LITERAL,
    NODE_VARIABLE_EXPRESSION,
    NODE_VAR_DECLARATION,
    NODE_PRINT_STATEMENT,
    NODE_IF_STATEMENT,
    NODE_WHILE_STATEMENT,
    NODE_ASSIGN_EXPRESSION,
    NODE_FUNCTION_DECLARATION, // <-- ADD
    NODE_CALL_EXPRESSION,      // <-- ADD
    NODE_RETURN_STATEMENT,
    NODE_LOGICAL_EXPRESSION,
    NODE_UNARY_EXPRESSION
} NodeType;

// A base structure that all AST nodes will have
typedef struct ASTNode {
    NodeType type;
} ASTNode;
typedef struct {
    ASTNode node;
    ASTNode* condition;
    ASTNode* body;
} WhileNode;
// --- AST Node Structures ---
typedef struct {
    ASTNode node;
    Token name;
    ASTNode* value;
} AssignNode;
typedef struct {
    ASTNode node;
    Token name;
    // For simplicity, we'll use a dynamic array of Tokens for parameters
    int arity; // Number of parameters
    Token* params;
    ASTNode* body;
} FunctionDeclNode;
typedef struct {
    ASTNode node;
    Token operator;
    ASTNode* right;
} UnaryNode;
typedef struct {
    ASTNode node;
    ASTNode* left;
    Token operator;
    ASTNode* right;
} LogicalNode;
typedef struct {
    ASTNode node;
    ASTNode* callee; // The expression that evaluates to a function
    int arg_count;
    ASTNode** arguments;
} CallNode;

typedef struct {
    ASTNode node;
    Token keyword; // The 'kthe' token for error reporting
    ASTNode* value; // The value to return (can be NULL)
} ReturnNode;
// A list of all statements in the program
typedef struct {
    ASTNode node;
    int statement_count;
    ASTNode** statements; // Dynamic array of statement nodes
} ProgramNode;

// A literal value like a number or a string
typedef struct {
    ASTNode node;
    Token value;
} LiteralNode;

// Using a variable in an expression
typedef struct {
    ASTNode node;
    Token name;
} VariableExprNode;

// A binary operation like 'x + y'
typedef struct {
    ASTNode node;
    ASTNode* left;
    Token operator;
    ASTNode* right;
} BinaryExprNode;

// A variable declaration like 'variabel x = 10;'
typedef struct {
    ASTNode node;
    Token name;
    ASTNode* initializer;
} VarDeclNode;

// A print statement like 'shkruaj x;'
typedef struct {
    ASTNode node;
    ASTNode* expression;
} PrintNode;

// An if statement like 'nese (x) { ... }'
typedef struct {
    ASTNode node;
    ASTNode* condition;
    ASTNode* then_branch;
    ASTNode* else_branch; // Can be NULL
} IfNode;


// --- Public "Constructor" Functions ---
ASTNode* create_program_node();
void add_statement_to_program(ProgramNode* program, ASTNode* statement);

ASTNode* create_binary_expr_node(ASTNode* left, Token op, ASTNode* right);
ASTNode* create_literal_node(Token value);
ASTNode* create_variable_expr_node(Token name);
ASTNode* create_var_decl_node(Token name, ASTNode* initializer);
ASTNode* create_print_node(ASTNode* expression);
ASTNode* create_if_node(ASTNode* condition, ASTNode* then_branch, ASTNode* else_branch);
ASTNode* create_while_node(ASTNode* condition, ASTNode* body);
ASTNode* create_assign_node(Token name, ASTNode* value);
ASTNode* create_function_decl_node(Token name, Token* params, int arity, ASTNode* body);
ASTNode* create_call_node(ASTNode* callee, ASTNode** arguments, int arg_count);
ASTNode* create_logical_node(ASTNode* left, Token op, ASTNode* right);
ASTNode* create_return_node(Token keyword, ASTNode* value);
ASTNode* create_unary_node(Token op, ASTNode* right);
void free_ast(ASTNode* node); // Function to clean up memory

#endif // AST_H