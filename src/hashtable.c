#include "../headers/hashtable.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

hashtable* create_hashtable() {
    hashtable* table = malloc(sizeof(hashtable));
    if (table == NULL) return nullptr;

    table->length = 0;
    table->capacity = INITIAL_CAPACITY;

    table->fields = calloc(table->capacity, sizeof(hash_field));
    if (table->fields == NULL) {
        free(table);
        return nullptr;
    }

    return table;
}

void free_hashtable(hashtable* table) {
    for (size_t i = 0; i < table->capacity; i++) {
        free((void*)table->fields[i].key);
        free((void*)table->fields[i].value);
    }
    free(table->fields);
    free(table);
}

uint64_t hash_key(const char* key) {
    uint64_t hash = FNV_OFFSET;
    for (const char* p = key; *p; p++) {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

void* ht_get(hashtable* table, const char* key) {
    uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(table->capacity - 1));

    while (table->fields[index].key != NULL) {
        if (strcmp(table->fields[index].key, key) == 0) {
            return table->fields[index].value;
        }
        index++;
        if (index >= table->capacity) {
            index = 0;
        }
    }

    return nullptr;
}

static const char* ht_set_entry(hash_field* fields, size_t capacity, const char* key, void* value, size_t* plength) {
    uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(capacity - 1));

    while (fields[index].key != NULL) {
        if (strcmp(fields[index].key, key) == 0) {
            fields[index].value = value;
            return fields[index].key;
        }

        index++;
        if (index >= capacity) {
            index = 0;
        }
    }

    if (plength != NULL) {
        key = strdup(key);
        if (key == NULL) {
            return nullptr;
        }
        (*plength)++;
    }
    fields[index].key = (char*)key;
    fields[index].value = value;
    return key;
}

static bool ht_resize(hashtable* table, size_t capacity) {
    if (capacity < table->capacity) {
        return false;
    }

    hash_field* new_fields = calloc(capacity, sizeof(hash_field));
    if (new_fields == NULL) {
        return false;
    }

    for (size_t i = 0; i < table->capacity; i++) {
        hash_field field = table->fields[i];
        if (field.key != NULL) {
            ht_set_entry(new_fields, capacity, field.key, field.value, nullptr);
        }
    }

    free(table->fields);
    table->fields = new_fields;
    table->capacity = capacity;
    return true;
}

const char* ht_set(hashtable* table, const char* key, void* value) {
    assert(value != NULL);
    if (value == NULL) {
        return nullptr;
    }

    if (table->length >= table->capacity / 2) {
        if (!ht_resize(table, table->capacity * 2)) {
            return nullptr;
        }
    }

    return ht_set_entry(table->fields, table->capacity, key, value, &table->length);
}

ht_iterator ht_iterator_init(hashtable* table) {
    ht_iterator iter;
    iter._table = table;
    iter._index = 0;
    return iter;
}

bool ht_iterator_next(ht_iterator* iter) {
    hashtable* table = iter->_table;
    while (iter->_index < table->capacity) {
        size_t index = iter->_index;
        iter->_index++;
        if (table->fields[index].key != NULL) {
            hash_field field = table->fields[index];
            iter->key = field.key;
            iter->value = field.value;
            return true;
        }
    }
    return false;
}
