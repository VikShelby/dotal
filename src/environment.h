#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "value.h"
#include "lexer.h" // For the Token struct

// A hash table entry
typedef struct {
    Token key;   // The variable name token
    Value value;
} Entry;

// The environment itself is a dynamic array of entries (a hash table)
// Use a forward declaration to break the circular reference
typedef struct Environment Environment;

struct Environment {
    int count;
    int capacity;
    Entry* entries;
    Environment* enclosing; // Now this is a simple pointer to the typedef'd type
};

void init_environment(Environment* env);
void free_environment(Environment* env);

// Defines a new variable or reassigns an existing one
void env_define(Environment* env, Token name, Value value);

// Gets a variable's value. Returns 'true' on success, 'false' if not found.
int env_get(Environment* env, Token name, Value* out_value);

#endif // ENVIRONMENT_H