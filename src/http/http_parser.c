#include "http_parser.h"
#include "definitions.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

request* parse_http_request(const char* raw_data) {
    request* req = create_request();
    if (req == NULL) {
        fprintf(stderr, "Request failed to initialize.\n");
        exit(1);
    }

    int index = parse_start_line(req->method, req->target, req->version, raw_data);
    if (index <= 0) {
        free_request(req);
        fprintf(stderr, "Bad request.\n");
        exit(1);
    }

    index += parse_fields(req->fields, raw_data + index);
    if (index <= 0) {
        free_request(req);
        fprintf(stderr, "Bad request.\n");
        exit(1);
    }

    req->body_length = parse_message_body(req, raw_data + index);
    return req;
}

static int parse_start_line(uint8_t* method, uint8_t* target, uint8_t* version, const char* raw_data) {
    size_t index = 0;
    size_t start_index = index;
    while (raw_data[index] != (uint8_t)' ') (index)++;
    if (index - start_index > MAX_SIZE_METHOD) return -1;
    memcpy(method, raw_data + start_index, index - start_index);
    (index)++;
    start_index = index;
    while (raw_data[index] != (uint8_t)' ') (index)++;
    if (index - start_index > MAX_SIZE_TARGET) return -1;
    memcpy(target, raw_data + start_index, index - start_index);
    (index)++;
    start_index = index;
    while (raw_data[index] != (uint8_t)'\r' && raw_data[index + 1] != (uint8_t)'\n') (index)++;
    if (index - start_index > MAX_SIZE_VERSION) return -1;
    memcpy(version, raw_data + start_index, index - start_index);
    index += 2;
    return (int)index;
}

static int parse_fields(hashtable* fields, const char* raw_data) {
    size_t index = 0;
    while (raw_data[index] != '\r' && raw_data[index + 1] != '\n') {
        size_t start_index = index;
        while (raw_data[index] != (uint8_t)':') (index)++;
        uint8_t *key = calloc(index - start_index, sizeof(uint8_t));
        memcpy(key, raw_data + start_index, index - start_index);
        index += 2;
        start_index = index;
        while (raw_data[index] != '\r' && raw_data[index + 1] != '\n') (index)++;
        uint8_t *value = calloc(index - start_index, sizeof(uint8_t));
        memcpy(value, raw_data + start_index, index - start_index);
        ht_set(fields, (char *)key, value);
        index += 2;
    }
    return (int)index + 2;
}

static size_t parse_message_body(request* req, const char* raw_data) {
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
            fprintf(stderr, "400: Bad request.\n");
            exit(-1);
        }
        return decode_body(req->body, encodings, encoding_length, raw_data);
    }

    if (content_length != NULL) {
        char* endptr;
        size_t length = (size_t)strtol((char*)content_length, &endptr, 10);
        req->body = calloc(length, sizeof(uint8_t));
        if (req->body == NULL) {
            fprintf(stderr, "Failed to allocate memory for body.\n");
        }
        if (length > 0) {
            while (raw_data[0] == '\n' || raw_data[0] == '\r' || raw_data[0] == ' ') {
                raw_data++;
            }
            for (size_t i = 0; i < length; i++) {
                req->body[i] = (uint8_t)raw_data[i];
            }
        }
        return length;
    }

    return 0;
}

static size_t get_encodings(uint8_t* data, transfer_encoding* encodings) {
    size_t index = 0;
    const char* ptr = strtok((char*)data, ",");
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

static size_t decode_body(uint8_t* body, const transfer_encoding* encodings, size_t length, const char* message) {
    for (size_t i = 0; i < length; i++) {
        switch (encodings[i]) {
            case CHUNKED:
                // return results of chunk body function
                return 0;
                break;
            default:
                printf("Unknown encoding type.\n");
                exit(-1);
                break;
        }
    }
    return 0;
}