// ===================================================================
// == REPLACE THE ENTIRE CONTENTS OF src/interpreter.c WITH THIS CODE ==
// ===================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "interpreter.h"
#include "value.h"
#include "ast.h"
#include "environment.h"

// --- Result & Error Handling ---

typedef struct {
    int is_return;
    Value value;
} InterpretResult;

static InterpretResult ok_result(Value v) { return (InterpretResult){0, v}; }
static InterpretResult return_result(Value v) { return (InterpretResult){1, v}; }
static Value unwrap(InterpretResult res) { return res.value; }

static void runtime_error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);
    exit(70);
}

static int is_truthy(Value value) {
    switch (value.type) {
        case VAL_BOOL:   return AS_BOOL(value);
        case VAL_NUMBER: return AS_NUMBER(value) != 0;
        default:         return 0;
    }
}

static int is_equal(Value a, Value b) {
    if (a.type != b.type) return 0;
    switch (a.type) {
        case VAL_BOOL:   return AS_BOOL(a) == AS_BOOL(b);
        case VAL_NUMBER: return AS_NUMBER(a) == AS_NUMBER(b);
        case VAL_STRING: return strcmp(AS_STRING(a), AS_STRING(b)) == 0;
        default: return 0;
    }
}

// --- Main Evaluation Function ---

static InterpretResult evaluate(ASTNode* node, Environment* env) {
    if (node == NULL) return ok_result(BOOL_VAL(0));

    switch (node->type) {
        case NODE_PROGRAM: {
            ProgramNode* program = (ProgramNode*)node;
            for (int i = 0; i < program->statement_count; i++) {
                InterpretResult result = evaluate(program->statements[i], env);
                if (result.is_return) return result;
            }
            return ok_result(BOOL_VAL(0));
        }

        case NODE_FUNCTION_DECLARATION: {
            FunctionDeclNode* func_node = (FunctionDeclNode*)node;
            Function* function = (Function*)malloc(sizeof(Function));
            function->declaration = (struct ASTNode*)func_node;
            env_define(env, func_node->name, FUNCTION_VAL(function));
            return ok_result(BOOL_VAL(0));
        }

        case NODE_RETURN_STATEMENT: {
            ReturnNode* ret_stmt = (ReturnNode*)node;
            Value val = BOOL_VAL(0);
            if (ret_stmt->value != NULL) {
                val = unwrap(evaluate(ret_stmt->value, env));
            }
            return return_result(val);
        }

        case NODE_VAR_DECLARATION: {
            VarDeclNode* decl = (VarDeclNode*)node;
            Value value = BOOL_VAL(0);
            if (decl->initializer != NULL) {
                value = unwrap(evaluate(decl->initializer, env));
            }
            env_define(env, decl->name, value);
            return ok_result(value);
        }

        case NODE_PRINT_STATEMENT: {
            PrintNode* print_stmt = (PrintNode*)node;
            Value value_to_print = unwrap(evaluate(print_stmt->expression, env));
            print_value(value_to_print);
            printf("\n");
            return ok_result(BOOL_VAL(0));
        }

        case NODE_IF_STATEMENT: {
            IfNode* if_stmt = (IfNode*)node;
            Value condition_val = unwrap(evaluate(if_stmt->condition, env));
            if (is_truthy(condition_val)) {
                return evaluate(if_stmt->then_branch, env);
            } else if (if_stmt->else_branch != NULL) {
                return evaluate(if_stmt->else_branch, env);
            }
            return ok_result(BOOL_VAL(0));
        }

        case NODE_WHILE_STATEMENT: {
            WhileNode* while_stmt = (WhileNode*)node;
            while (is_truthy(unwrap(evaluate(while_stmt->condition, env)))) {
                InterpretResult result = evaluate(while_stmt->body, env);
                if (result.is_return) return result;
            }
            return ok_result(BOOL_VAL(0));
        }

        case NODE_ASSIGN_EXPRESSION: {
            AssignNode* assign_expr = (AssignNode*)node;
            Value value = unwrap(evaluate(assign_expr->value, env));
            env_define(env, assign_expr->name, value);
            return ok_result(value);
        }

        case NODE_CALL_EXPRESSION: {
            CallNode* call = (CallNode*)node;
            Value callee = unwrap(evaluate(call->callee, env));
            if (!IS_FUNCTION(callee)) {
                runtime_error("Can only call functions.");
            }
            Function* function = AS_FUNCTION(callee);
            FunctionDeclNode* decl = (FunctionDeclNode*)function->declaration;
            if (call->arg_count != decl->arity) {
                runtime_error("Expected %d arguments but got %d.", decl->arity, call->arg_count);
            }
            Environment frame;
            init_environment(&frame);
            frame.enclosing = env;
            for (int i = 0; i < decl->arity; i++) {
                Value arg_val = unwrap(evaluate(call->arguments[i], env));
                env_define(&frame, decl->params[i], arg_val);
            }
            InterpretResult result = evaluate(decl->body, &frame);
            free_environment(&frame);
            return ok_result(result.value);
        }
        case NODE_LOGICAL_EXPRESSION: {
            LogicalNode* logical_expr = (LogicalNode*)node;

            Value left = unwrap(evaluate(logical_expr->left, env));

            // Short-circuit for 'ose' (OR)
            if (logical_expr->operator.type == TOKEN_OR) {
                if (is_truthy(left)) return ok_result(left);
            } 
            // Short-circuit for 'dhe' (AND)
            else { // Must be TOKEN_AND
                if (!is_truthy(left)) return ok_result(left);
            }
            
            // If we didn't short-circuit, evaluate the right side
            return evaluate(logical_expr->right, env);
        }

        case NODE_BINARY_EXPRESSION: {
            BinaryExprNode* bin_op = (BinaryExprNode*)node;
            Value left = unwrap(evaluate(bin_op->left, env));
            Value right = unwrap(evaluate(bin_op->right, env));

            switch (bin_op->operator.type) {
                // --- Special Case: Addition and Concatenation ---
                case TOKEN_PLUS: {
                    if (IS_NUMBER(left) && IS_NUMBER(right)) {
                        return ok_result(NUMBER_VAL(AS_NUMBER(left) + AS_NUMBER(right)));
                    }
                    if (IS_STRING(left) && IS_STRING(right)) {
                        // Concatenate strings
                        char* left_str = AS_STRING(left);
                        char* right_str = AS_STRING(right);
                        size_t left_len = strlen(left_str);
                        size_t right_len = strlen(right_str);
                        
                        // Malloc new memory for the result
                        char* result = (char*)malloc(left_len + right_len + 1);
                        
                        // Copy the strings into the new buffer
                        memcpy(result, left_str, left_len);
                        memcpy(result + left_len, right_str, right_len);
                        result[left_len + right_len] = '\0';

                        return ok_result(STRING_VAL(result));
                    }
                    runtime_error("Operands for '+' must be two numbers or two strings.");
                }

                // --- Other Arithmetic Operators (Numbers Only) ---
                case TOKEN_MINUS:
                case TOKEN_STAR:
                case TOKEN_SLASH: {
                    if (!IS_NUMBER(left) || !IS_NUMBER(right)) {
                        runtime_error("Operands must be numbers.");
                    }
                    // Fall through to the comparisons' number check
                }

                // --- Comparison Operators (Numbers or Bools) ---
                case TOKEN_GREATER:
                case TOKEN_GREATER_EQUAL:
                case TOKEN_LESS:
                case TOKEN_LESS_EQUAL: {
                    if (!IS_NUMBER(left) || !IS_NUMBER(right)) {
                        runtime_error("Operands must be numbers for comparison.");
                    }
                    switch (bin_op->operator.type) {
                         case TOKEN_MINUS:         return ok_result(NUMBER_VAL(AS_NUMBER(left) - AS_NUMBER(right)));
                         case TOKEN_STAR:          return ok_result(NUMBER_VAL(AS_NUMBER(left) * AS_NUMBER(right)));
                         case TOKEN_SLASH:
                            if (AS_NUMBER(right) == 0) runtime_error("Division by zero.");
                            return ok_result(NUMBER_VAL(AS_NUMBER(left) / AS_NUMBER(right)));
                         case TOKEN_GREATER:       return ok_result(BOOL_VAL(AS_NUMBER(left) > AS_NUMBER(right)));
                         case TOKEN_GREATER_EQUAL: return ok_result(BOOL_VAL(AS_NUMBER(left) >= AS_NUMBER(right)));
                         case TOKEN_LESS:          return ok_result(BOOL_VAL(AS_NUMBER(left) < AS_NUMBER(right)));
                         case TOKEN_LESS_EQUAL:    return ok_result(BOOL_VAL(AS_NUMBER(left) <= AS_NUMBER(right)));
                    }
                }

                // --- Equality Operators (All Types) ---
                case TOKEN_EQUAL_EQUAL: return ok_result(BOOL_VAL(is_equal(left, right)));
                case TOKEN_BANG_EQUAL:  return ok_result(BOOL_VAL(!is_equal(left, right)));
            }
        }

        case NODE_VARIABLE_EXPRESSION: {
            VariableExprNode* expr = (VariableExprNode*)node;
            Value value;
            if (!env_get(env, expr->name, &value)) {
                runtime_error("Undefined variable '%.*s'.", expr->name.length, expr->name.start);
            }
            return ok_result(value);
        }

        case NODE_LITERAL: {
            LiteralNode* literal = (LiteralNode*)node;
            switch (literal->value.type) {
                case TOKEN_NUMBER: return ok_result(NUMBER_VAL(strtod(literal->value.start, NULL)));
                case TOKEN_TRUE: return ok_result(BOOL_VAL(1));
                case TOKEN_FALSE: return ok_result(BOOL_VAL(0));
                case TOKEN_STRING: {
                    int length = literal->value.length - 2;
                    char* chars = (char*)malloc(length + 1);
                    memcpy(chars, literal->value.start + 1, length);
                    chars[length] = '\0';
                    return ok_result(STRING_VAL(chars));
                }
                default: break;
            }
        }
    }
    return ok_result(BOOL_VAL(0));
}

void interpret(ASTNode* program) {
    if (program == NULL) return;
    Environment env;
    init_environment(&env);
    evaluate(program, &env);
    free_environment(&env);
}