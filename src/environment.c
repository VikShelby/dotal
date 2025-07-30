#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "environment.h"

void init_environment(Environment* env) {
    env->count = 0;
    env->capacity = 0;
    env->entries = NULL;
    env->enclosing = NULL; 
}

void free_environment(Environment* env) {
    free(env->entries);
    init_environment(env);
}

// The core lookup function. Finds where a key should be in the table.
static Entry* find_entry(Entry* entries, int capacity, Token key) {
    if (capacity == 0) return NULL;
    // Use the FNV-1a hashing algorithm to get an index
    uint32_t hash = 2166136261u;
    for (int i = 0; i < key.length; i++) {
        hash ^= (uint8_t)key.start[i];
        hash *= 16777619;
    }
    uint32_t index = hash % capacity;

    for (;;) {
        Entry* entry = &entries[index];
        // If we find an empty slot or the correct key, we're done.
        // A key with length 0 marks an empty, usable slot.
        if (entry->key.length == 0 || 
           (entry->key.length == key.length && memcmp(entry->key.start, key.start, key.length) == 0)) {
            return entry;
        }
        // Linear probing: if we have a collision, try the next slot.
        index = (index + 1) % capacity;
    }
}

int env_get(Environment* env, Token name, Value* out_value) {
    Entry* entry = find_entry(env->entries, env->capacity, name);
    if (entry != NULL && entry->key.length != 0) {
        *out_value = entry->value;
        return 1;
    }

    // If not found, check the parent scope
    if (env->enclosing != NULL) {
        return env_get(env->enclosing, name, out_value);
    }

    return 0;
}
static void adjust_capacity(Environment* env, int capacity) {
    Entry* entries = (Entry*)malloc(sizeof(Entry) * capacity);
    for (int i = 0; i < capacity; i++) {
        entries[i].key.length = 0; // Mark all slots as empty
    }

    // Re-insert all existing entries into the new, larger table
    for (int i = 0; i < env->capacity; i++) {
        Entry* old_entry = &env->entries[i];
        if (old_entry->key.length != 0) {
            Entry* dest = find_entry(entries, capacity, old_entry->key);
            dest->key = old_entry->key;
            dest->value = old_entry->value;
        }
    }

    free(env->entries);
    env->entries = entries;
    env->capacity = capacity;
}

void env_define(Environment* env, Token name, Value value) {
    // If the table is getting full, grow it.
    if (env->count + 1 > env->capacity * 0.75) {
        int old_capacity = env->capacity;
        int new_capacity = old_capacity < 8 ? 8 : old_capacity * 2;
        adjust_capacity(env, new_capacity);
    }

    Entry* entry = find_entry(env->entries, env->capacity, name);
    // If the key is new, increment the count.
    if (entry->key.length == 0) {
        env->count++;
    }

    entry->key = name;
    entry->value = value;
}