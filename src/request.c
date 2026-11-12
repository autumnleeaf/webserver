#include "request.h"

#include <stdlib.h>
#include <string.h>

request* create_request() {
    request* req = malloc(sizeof(request));
    if (req == NULL) return nullptr;

    req->method = calloc(MAX_SIZE_METHOD, sizeof(uint8_t));
    if (req->method == NULL) {
        free(req);
        return nullptr;
    }

    req->target = calloc(MAX_SIZE_TARGET, sizeof(uint8_t));
    if (req->version == NULL) {
        free(req->method);
        free(req);
        return nullptr;
    }

    req->version = calloc(MAX_SIZE_VERSION, sizeof(uint8_t));
    if (req->version == NULL) {
        free(req->method);
        free(req->target);
        free(req);
        return nullptr;
    }

    req->fields = create_hashtable();
    if (req->fields == NULL) {
        free(req->method);
        free(req->target);
        free(req->version);
        free(req);
        return nullptr;
    }

    return req;
}

void free_request(request* req) {
    free(req->method);
    free(req->target);
    free(req->version);
    free_hashtable(req->fields);
    if (req->body != NULL) free(req->body);
    free(req);
}

bool parse_start_line(const request* req, const uint8_t* message, size_t* index) {
    size_t start_index = *index;
    while (message[*index] != (uint8_t)' ') (*index)++;
    if (*index - start_index > MAX_SIZE_METHOD) return false;
    memcpy(req->method, message + start_index, *index - start_index);
    (*index)++;
    start_index = *index;
    while (message[*index] != (uint8_t)' ') (*index)++;
    if (*index - start_index > MAX_SIZE_TARGET) return false;
    memcpy(req->target, message + start_index, *index - start_index);
    (*index)++;
    start_index = *index;
    while (message[*index] != (uint8_t)'\r' && message[*index + 1] != (uint8_t)'\n') (*index)++;
    if (*index - start_index > MAX_SIZE_VERSION) return false;
    memcpy(req->version, message + start_index, *index - start_index);
    *index += 2;
    return true;
}

bool parse_fields(const request* req, const uint8_t* message, size_t* index) {
    while (message[*index] != '\r' && message[*index + 1] != '\n') {
        size_t start_index = *index;
        while (message[*index] != (uint8_t)':') (*index)++;
        uint8_t *key = calloc(*index - start_index, sizeof(uint8_t));
        memcpy(key, message + start_index, *index - start_index);
        *index += 2;
        start_index = *index;
        while (message[*index] != '\r' && message[*index + 1] != '\n') (*index)++;
        uint8_t *value = calloc(*index - start_index, sizeof(uint8_t));
        memcpy(value, message + start_index, *index - start_index);
        ht_set(req->fields, (char *)key, value);
        *index += 2;
    }
    return true;
}