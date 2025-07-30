
#ifndef AST_H
#define AST_H

#include "lexer.h" 

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
    NODE_FUNCTION_DECLARATION, 
    NODE_CALL_EXPRESSION,      
    NODE_RETURN_STATEMENT,
    NODE_LOGICAL_EXPRESSION,
    NODE_UNARY_EXPRESSION
} NodeType;


typedef struct ASTNode {
    NodeType type;
} ASTNode;
typedef struct {
    ASTNode node;
    ASTNode* condition;
    ASTNode* body;
} WhileNode;
typedef struct {
    ASTNode node;
    Token name;
    ASTNode* value;
} AssignNode;
typedef struct {
    ASTNode node;
    Token name;
    int arity;
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
    ASTNode* callee; 
    int arg_count;
    ASTNode** arguments;
} CallNode;

typedef struct {
    ASTNode node;
    Token keyword; 
    ASTNode* value; 
} ReturnNode;

typedef struct {
    ASTNode node;
    int statement_count;
    ASTNode** statements; 
} ProgramNode;

typedef struct {
    ASTNode node;
    Token value;
} LiteralNode;

typedef struct {
    ASTNode node;
    Token name;
} VariableExprNode;

typedef struct {
    ASTNode node;
    ASTNode* left;
    Token operator;
    ASTNode* right;
} BinaryExprNode;

typedef struct {
    ASTNode node;
    Token name;
    ASTNode* initializer;
} VarDeclNode;

typedef struct {
    ASTNode node;
    ASTNode* expression;
} PrintNode;
typedef struct {
    ASTNode node;
    ASTNode* condition;
    ASTNode* then_branch;
    ASTNode* else_branch;
} IfNode;


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
void free_ast(ASTNode* node); 

#endif 