#include "request.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

request* create_request() {
    request* req = malloc(sizeof(request));
    if (req == NULL) return nullptr;

    req->method = calloc(MAX_SIZE_METHOD, sizeof(uint8_t));
    if (req->method == NULL) {
        free(req);
        printf("Method failed to allocate.\n");
        return nullptr;
    }

    req->target = calloc(MAX_SIZE_TARGET, sizeof(uint8_t));
    if (req->target == NULL) {
        free(req->method);
        free(req);
        printf("Target failed to allocate.\n");
        return nullptr;
    }

    req->version = calloc(MAX_SIZE_VERSION, sizeof(uint8_t));
    if (req->version == NULL) {
        free(req->method);
        free(req->target);
        free(req);
        printf("Version failed to allocate.\n");
        return nullptr;
    }

    req->fields = create_hashtable();
    if (req->fields == NULL) {
        free(req->method);
        free(req->target);
        free(req->version);
        free(req);
        printf("Hash table failed to allocate.\n");
        return nullptr;
    }

    req->body = nullptr;

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

size_t parse_message_body(request* req, const uint8_t* message, size_t index) {
    uint8_t* raw_transfer_encoding = ht_get(req->fields, "Transfer-Encoding");
    uint8_t* content_length = ht_get(req->fields, "Content-Length");

    if (raw_transfer_encoding != NULL && content_length != NULL) {
        printf("Request smuggling detected.");
        exit(-1);
    }

    if (raw_transfer_encoding != NULL) {
        transfer_encoding* encodings = calloc(8, sizeof(transfer_encoding));
        size_t encoding_length = get_encodings(raw_transfer_encoding, encodings);
        if (encodings[encoding_length - 1] != CHUNKED) {
            // Return a 400 Bad Request error
            printf("400: Bad request.\n");
            exit(-1);
        }
        uint8_t* body = calloc(2048 - index, sizeof(uint8_t));
        req->body_length = decode_body(req, message + index, encodings, encoding_length);
        req->body = body;
    }

    if (content_length != NULL) {
        char* endptr;
        req->body_length = (size_t)strtol((char*)content_length, &endptr, 10);
        req->body = calloc(req->body_length, sizeof(uint8_t));
        if (req->body == NULL) req->body_length = 0;
        for (size_t i = 0; i < req->body_length; i++) {
            req->body[i] = message[index + i];
        }
        return req->body_length;
    }

    return 0;
}

static size_t get_encodings(uint8_t* data, transfer_encoding* encodings) {
    size_t index = 0;
    char* ptr = strtok((char*)data, ",");
    while (ptr != NULL) {
        if (strcmp(ptr, "chunked") == 0) {
            encodings[index] = CHUNKED;
        } else if (strcmp(ptr, "compress") == 0) {
            encodings[index] = COMPRESS;
        } else if (strcmp(ptr, "deflate") == 0) {
            encodings[index] = DEFLATE;
        } else if (strcmp(ptr, "gzip") == 0) {
            encodings[index] = GZIP;
        } else if (strcmp(ptr, "identity") == 0) {
            encodings[index] = IDENTITY;
        } else if (strcmp(ptr, "trailers") == 0) {
            encodings[index] = TRAILERS;
        } else if (strcmp(ptr, "x-compress") == 0) {
            encodings[index] = X_COMPRESS;
        } else if (strcmp(ptr, "x-gzip") == 0) {
            encodings[index] = X_GZIP;
        } else {
            printf("Unknown encoding type.\n");
            exit(-1);
        }
        ptr = strtok(nullptr, ",");
        index++;
    }
    return index;
}

static size_t decode_body(request* req, const uint8_t* message, const transfer_encoding* encodings, size_t length) {
    for (size_t i = 0; i < length; i++) {
        switch (encodings[i]) {
            case CHUNKED:
                return chunk_body(req, message);
                break;
            default:
                printf("Unknown encoding type.\n");
                exit(-1);
                break;
        }
    }
    return 0;
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
    *index += 2;
    return true;
}