#include "http_parser.h"
#include "definitions.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t parse_message_body(uint8_t* body, const hashtable* fields, const uint8_t* message) {
    uint8_t* raw_transfer_encoding = ht_get(fields, "Transfer-Encoding");
    uint8_t* content_length = ht_get(fields, "Content-Length");

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
        return decode_body(body, message, encodings, encoding_length);
    }

    if (content_length != NULL) {
        char* endptr;
        size_t length = (size_t)strtol((char*)content_length, &endptr, 10);
        body = calloc(length, sizeof(uint8_t));
        if (body == NULL) length = 0;
        for (size_t i = 0; i < length; i++) {
            body[i] = message[i];
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

static size_t decode_body(uint8_t* body, const transfer_encoding* encodings, size_t length, const uint8_t* message) {
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

int parse_start_line(uint8_t* method, uint8_t* target, uint8_t* version, const uint8_t* message) {
    size_t index = 0;
    size_t start_index = index;
    while (message[index] != (uint8_t)' ') (index)++;
    if (index - start_index > MAX_SIZE_METHOD) return -1;
    memcpy(method, message + start_index, index - start_index);
    (index)++;
    start_index = index;
    while (message[index] != (uint8_t)' ') (index)++;
    if (index - start_index > MAX_SIZE_TARGET) return -1;
    memcpy(target, message + start_index, index - start_index);
    (index)++;
    start_index = index;
    while (message[index] != (uint8_t)'\r' && message[index + 1] != (uint8_t)'\n') (index)++;
    if (index - start_index > MAX_SIZE_VERSION) return -1;
    memcpy(version, message + start_index, index - start_index);
    index += 2;
    return (int)index;
}

int parse_fields(hashtable* fields, const uint8_t* message) {
    size_t index = 0;
    while (message[index] != '\r' && message[index + 1] != '\n') {
        size_t start_index = index;
        while (message[index] != (uint8_t)':') (index)++;
        uint8_t *key = calloc(index - start_index, sizeof(uint8_t));
        memcpy(key, message + start_index, index - start_index);
        index += 2;
        start_index = index;
        while (message[index] != '\r' && message[index + 1] != '\n') (index)++;
        uint8_t *value = calloc(index - start_index, sizeof(uint8_t));
        memcpy(value, message + start_index, index - start_index);
        ht_set(fields, (char *)key, value);
        index += 2;
    }
    return (int)index + 2;
}