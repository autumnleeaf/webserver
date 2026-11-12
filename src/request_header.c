#include "request_header.h"

#include <stdlib.h>
#include <string.h>

request_header* create_req_header() {
    request_header* req_header = malloc(sizeof(request_header));
    if (req_header == NULL) return nullptr;

    req_header->method = calloc(MAX_SIZE_METHOD, sizeof(uint8_t));
    if (req_header->method == NULL) {
        free(req_header);
        return nullptr;
    }

    req_header->target = calloc(MAX_SIZE_TARGET, sizeof(uint8_t));
    if (req_header->version == NULL) {
        free(req_header->method);
        free(req_header);
        return nullptr;
    }

    req_header->version = calloc(MAX_SIZE_VERSION, sizeof(uint8_t));
    if (req_header->version == NULL) {
        free(req_header->method);
        free(req_header->target);
        free(req_header);
        return nullptr;
    }

    req_header->fields = create_hashtable();
    if (req_header->fields == NULL) {
        free(req_header->method);
        free(req_header->target);
        free(req_header->version);
        free(req_header);
        return nullptr;
    }

    return req_header;
}

void free_req_header(request_header* req_header) {
    free(req_header->method);
    free(req_header->target);
    free(req_header->version);
    free_hashtable(req_header->fields);
    free(req_header);
}

bool parse_start_line(const request_header* req_header, const uint8_t* message, size_t* index) {
    size_t start_index = *index;
    while (message[*index] != (uint8_t)' ') (*index)++;
    memcpy(req_header->method, message + start_index, *index - start_index);
    (*index)++;
    start_index = *index;
    while (message[*index] != (uint8_t)' ') (*index)++;
    memcpy(req_header->target, message + start_index, *index - start_index);
    (*index)++;
    start_index = *index;
    while (message[*index] != (uint8_t)'\r' && message[*index + 1] != (uint8_t)'\n') (*index)++;
    memcpy(req_header->version, message + start_index, *index - start_index);
    *index += 2;
    return true;
}

bool parse_fields(const request_header* req_header, const uint8_t* message, size_t* index) {
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
        ht_set(req_header->fields, (char *)key, value);
        *index += 2;
    }
    return true;
}