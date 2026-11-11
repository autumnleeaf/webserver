//
// Created by autumn on 11/10/26.
//

#ifndef WEBSERVER_HASHMAP_H
#define WEBSERVER_HASHMAP_H
#include <stddef.h>
#include <stdint.h>

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL
#define INITIAL_CAPACITY 16

typedef struct {
    char* key;
    char* value;
} hash_field;

typedef struct {
    hash_field* fields;
    size_t capacity;
    size_t length;
} hash_table;

typedef struct {
    const char* key;
    void* value;

    hash_table* _table;
    size_t _index;
} hash_table_iterator;

hash_table* create_hash_table(void);

void free_hash_table(hash_table* table);

static uint64_t hash_key(const char* key);

void* hash_table_get(hash_table* table, const char* key);

const char* hash_table_set(hash_table* table, const char* key, void* value);

static const char* hash_table_set_entry(hash_field* fields, size_t capacity, const char* key, void* value, size_t* plength);

static bool hash_table_resize(hash_table* table, size_t capacity);

hash_table_iterator hash_table_iterator_init(hash_table* table);

bool hash_table_iterator_next(hash_table_iterator* iter);

#endif //WEBSERVER_HASHMAP_H
