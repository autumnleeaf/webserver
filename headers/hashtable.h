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
} hashtable;

typedef struct {
    const char* key;
    void* value;

    hashtable* _table;
    size_t _index;
} ht_iterator;

hashtable* create_hashtable();

void free_hashtable(hashtable* table);

static uint64_t hash_key(const char* key);

void* ht_get(const hashtable* table, const char* key);

const char* ht_set(hashtable* table, const char* key, void* value);

static const char* ht_set_entry(hash_field* fields, size_t capacity, const char* key, void* value, size_t* plength);

static bool ht_resize(hashtable* table, size_t capacity);

ht_iterator ht_iterator_init(hashtable* table);

bool ht_iterator_next(ht_iterator* iter);

#endif //WEBSERVER_HASHMAP_H
