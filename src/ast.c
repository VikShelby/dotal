#include <stdlib.h>
#include <stdio.h>
#include "ast.h"

// --- Program Node ---
ASTNode* create_program_node() {
    ProgramNode* node = (ProgramNode*)malloc(sizeof(ProgramNode));
    node->node.type = NODE_PROGRAM;
    node->statement_count = 0;
    node->statements = NULL;
    return (ASTNode*)node;
}

void add_statement_to_program(ProgramNode* program, ASTNode* statement) {
    program->statement_count++;
    program->statements = (ASTNode**)realloc(program->statements, sizeof(ASTNode*) * program->statement_count);
    program->statements[program->statement_count - 1] = statement;
}

// --- Expression & Statement Nodes ---
ASTNode* create_binary_expr_node(ASTNode* left, Token op, ASTNode* right) {
    BinaryExprNode* node = (BinaryExprNode*)malloc(sizeof(BinaryExprNode));
    node->node.type = NODE_BINARY_EXPRESSION;
    node->left = left;
    node->operator = op;
    node->right = right;
    return (ASTNode*)node;
}
ASTNode* create_if_node(ASTNode* condition, ASTNode* then_branch, ASTNode* else_branch) {
    IfNode* node = (IfNode*)malloc(sizeof(IfNode));
    node->node.type = NODE_IF_STATEMENT;
    node->condition = condition;
    node->then_branch = then_branch;
    node->else_branch = else_branch;
    return (ASTNode*)node;
}
ASTNode* create_literal_node(Token value) {
    LiteralNode* node = (LiteralNode*)malloc(sizeof(LiteralNode));
    node->node.type = NODE_LITERAL;
    node->value = value;
    return (ASTNode*)node;
}

ASTNode* create_variable_expr_node(Token name) {
    VariableExprNode* node = (VariableExprNode*)malloc(sizeof(VariableExprNode));
    node->node.type = NODE_VARIABLE_EXPRESSION;
    node->name = name;
    return (ASTNode*)node;
}

ASTNode* create_var_decl_node(Token name, ASTNode* initializer) {
    VarDeclNode* node = (VarDeclNode*)malloc(sizeof(VarDeclNode));
    node->node.type = NODE_VAR_DECLARATION;
    node->name = name;
    node->initializer = initializer;
    return (ASTNode*)node;
}

ASTNode* create_print_node(ASTNode* expression) {
    PrintNode* node = (PrintNode*)malloc(sizeof(PrintNode));
    node->node.type = NODE_PRINT_STATEMENT;
    node->expression = expression;
    return (ASTNode*)node;
}
ASTNode* create_while_node(ASTNode* condition, ASTNode* body) {
    WhileNode* node = (WhileNode*)malloc(sizeof(WhileNode));
    node->node.type = NODE_WHILE_STATEMENT;
    node->condition = condition;
    node->body = body;
    return (ASTNode*)node;
}
ASTNode* create_assign_node(Token name, ASTNode* value) {
    AssignNode* node = (AssignNode*)malloc(sizeof(AssignNode));
    node->node.type = NODE_ASSIGN_EXPRESSION;
    node->name = name;
    node->value = value;
    return (ASTNode*)node;
}
ASTNode* create_function_decl_node(Token name, Token* params, int arity, ASTNode* body) {
    FunctionDeclNode* node = (FunctionDeclNode*)malloc(sizeof(FunctionDeclNode));
    node->node.type = NODE_FUNCTION_DECLARATION;
    node->name = name;
    node->params = params;
    node->arity = arity;
    node->body = body;
    return (ASTNode*)node;
}

ASTNode* create_call_node(ASTNode* callee, ASTNode** arguments, int arg_count) {
    CallNode* node = (CallNode*)malloc(sizeof(CallNode));
    node->node.type = NODE_CALL_EXPRESSION;
    node->callee = callee;
    node->arguments = arguments;
    node->arg_count = arg_count;
    return (ASTNode*)node;
}

ASTNode* create_return_node(Token keyword, ASTNode* value) {
    ReturnNode* node = (ReturnNode*)malloc(sizeof(ReturnNode));
    node->node.type = NODE_RETURN_STATEMENT;
    node->keyword = keyword;
    node->value = value;
    return (ASTNode*)node;
}
ASTNode* create_logical_node(ASTNode* left, Token op, ASTNode* right) {
    LogicalNode* node = (LogicalNode*)malloc(sizeof(LogicalNode));
    node->node.type = NODE_LOGICAL_EXPRESSION;
    node->left = left;
    node->operator = op;
    node->right = right;
    return (ASTNode*)node;
}
ASTNode* create_unary_node(Token op, ASTNode* right) {
    UnaryNode* node = (UnaryNode*)malloc(sizeof(UnaryNode));
    node->node.type = NODE_UNARY_EXPRESSION;
    node->operator = op;
    node->right = right;
    return (ASTNode*)node;
}

// --- Memory Management ---
void free_ast(ASTNode* node) {
    if (node == NULL) return;

    switch (node->type) {
        case NODE_PROGRAM: {
            ProgramNode* program = (ProgramNode*)node;
            for (int i = 0; i < program->statement_count; i++) {
                free_ast(program->statements[i]);
            }
            free(program->statements);
            break;
        }
        case NODE_BINARY_EXPRESSION: {
            BinaryExprNode* bin_op = (BinaryExprNode*)node;
            free_ast(bin_op->left);
            free_ast(bin_op->right);
            break;
        }
        case NODE_UNARY_EXPRESSION: {
             UnaryNode* unary_expr = (UnaryNode*)node;
              free_ast(unary_expr->right);
             break;
        }
        case NODE_VAR_DECLARATION: {
            VarDeclNode* var_decl = (VarDeclNode*)node;
            free_ast(var_decl->initializer);
            break;
        }
        case NODE_PRINT_STATEMENT: {
            PrintNode* print_stmt = (PrintNode*)node;
            free_ast(print_stmt->expression);
            break;
        }
        case NODE_LOGICAL_EXPRESSION: {
            LogicalNode* logical_expr = (LogicalNode*)node;
            free_ast(logical_expr->left);
            free_ast(logical_expr->right);
            break;
        }
        case NODE_IF_STATEMENT: {
            IfNode* if_stmt = (IfNode*)node;
            free_ast(if_stmt->condition);
            free_ast(if_stmt->then_branch);
            free_ast(if_stmt->else_branch);
            break;
        }
        case NODE_WHILE_STATEMENT: {
            WhileNode* while_stmt = (WhileNode*)node;
            free_ast(while_stmt->condition);
            free_ast(while_stmt->body);
            break;
        }
case NODE_ASSIGN_EXPRESSION: {
    AssignNode* assign_expr = (AssignNode*)node;
    free_ast(assign_expr->value);
    break;
}   case NODE_FUNCTION_DECLARATION: {
            FunctionDeclNode* func = (FunctionDeclNode*)node;
         
            free(func->params);
            free_ast(func->body);
            break;
        }
        case NODE_CALL_EXPRESSION: {
            CallNode* call = (CallNode*)node;
            free_ast(call->callee);
            for (int i = 0; i < call->arg_count; i++) {
                free_ast(call->arguments[i]);
            }
            free(call->arguments);
            break;
        }
        case NODE_RETURN_STATEMENT: {
            ReturnNode* ret = (ReturnNode*)node;
            free_ast(ret->value);
            break;
        }
        case NODE_VARIABLE_EXPRESSION:
        case NODE_LITERAL:
            break;
    }
    free(node);
}